#include <iostream>
#include <ratio>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <tuple>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

#define MASTER 0
#define QUIT 1

using namespace std;
using namespace std::chrono;

float getFunctionData(int functionId, float x, int intensity) {
  switch (functionId)
  {
    case 1:
      return f1(x, intensity);
    case 2:
      return f2(x, intensity);
    case 3:
      return f3(x, intensity);
    case 4:
      return f4(x, intensity);
    default:
      return -1;
  }
}

float calculateIntegral(int start, int end, int functionId, int intensity, float a, float b, long n)
{
  float result = 0.0;
  float width = (b - a) / float(n);
  for (int i = start; i < end; i++)
  {
    float x = (a + (i + 0.5) * width);
    float func = getFunctionData( functionId, x, intensity );
    result = result + (width * func);
  }
  return result;
}

std::tuple<int, int> getData(int index, long size, int nprocess)
{
  nprocess = nprocess - 1;
  int chunk = size / (nprocess);
  int start = index * chunk;
  int end = start + chunk;

  if ((size % nprocess != 0) && (end > size))
  {
    end = size;
  }
  return std::make_tuple(start, end);
}

float masterTask(long size, int nprocess)
{
  float finalResult = 0.0;
  int index = -1;
  int work_sent = 0;
  int start, end = 0;
  float result = 0.0;

  for (int i = 1; i < nprocess; i++)
  {
    if (end < size)
    {
      index++;
      work_sent++;
      std::tie(start, end) = getData(index, size, nprocess);
      int work[2] = {0};
      work[0] = start;
      work[1] = end;
      MPI_Send(work, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else
    {
      MPI_Send(0, 0, MPI_INT, i, QUIT, MPI_COMM_WORLD);
    }
  }

  while (work_sent != 0)
  {
    MPI_Status status;

    MPI_Recv(&result, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    
    int id = status.MPI_SOURCE;
    finalResult += result;
    work_sent--;

    if (end < size)
    {
      index++;
      work_sent++;
      std::tie(start, end) = getData(index, size, nprocess);
      int work[2] = {0};
      work[0] = start;
      work[1] = end;
      MPI_Send(work, 2, MPI_INT, id, 0, MPI_COMM_WORLD);
    }
    else
    {
      MPI_Send(0, 0, MPI_INT, id, QUIT, MPI_COMM_WORLD);
    }
  }
  return finalResult;
}

void workerTask(int functionId, int intensity, float a, float b, long size)
{
  float result = 0.0;
  int work[2] = {0};
  MPI_Status status;
  while (1)
  {
    MPI_Recv(work, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int tag = status.MPI_TAG;
    if (tag != QUIT)
    {
      int start = work[0];
      int end = work[1];
      result = calculateIntegral(start, end, functionId, intensity, a, b, size);
      MPI_Send(&result, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
      return;
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc < 6)
  {
    std::cerr << "usage: mpirun " << argv[0] << " <functionid> <a> <b> <n> <intensity>" << std::endl;
    return -1;
  }

  int functionId, intensity, nprocess, rank;
  long int size;
  float a, b;
  float result = 0.0;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  functionId = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  size = atoi(argv[4]);
  intensity = atoi(argv[5]);

  high_resolution_clock::time_point start = high_resolution_clock::now();

  if (rank == 0)
  {
    result = masterTask(size, nprocess);
  }
  else
  {
    workerTask(functionId, intensity, a, b, size);
  }

  high_resolution_clock::time_point end = high_resolution_clock::now();
  duration<double> elapsed = duration_cast<duration<double> >(end - start);
  if (rank == 0)
  {
    std::cout << result << std::endl;
    std::cerr << elapsed.count() << std::endl;
  }

  MPI_Finalize();
  return 0;
}
