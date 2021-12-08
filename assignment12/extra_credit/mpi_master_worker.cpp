#include <iostream>
#include <ratio>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <tuple>

#define ARR_START_SEND 100
#define ARR_START_RECV 100
#define ARR_END_SEND 101
#define ARR_END_RECV 101
#define RESULT_TAG 1000
#define MASTER 0

using namespace std;

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

#define INITIAL_WORK_REQ 1
#define QUIT 1

float func_selector(int func_id, float x, int intensity)
{
  switch (func_id)
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

float computeIntegral(int start, int end, int function_id, int intensity, float a, float b, long n)
{
  float result = 0.0;
  float width = (b - a) / float(n);
  for (int i = start; i < end; i++)
  {
    float x = (a + (i + 0.5) * width);
    float func = func_selector(function_id, x, intensity);
    result = result + (width * func);
  }
  return result;
}

std::tuple<int, int> getData(int req_id, long n, int nbprocess)
{
  nbprocess = nbprocess - 1;
  int gran = n / (3 * nbprocess);
  int start_ptr = req_id * gran;
  int end_ptr = start_ptr + gran;
  if ((n % (3 * nbprocess) != 0) && (end_ptr > n))
  {
    end_ptr = n;
  }
  return std::make_tuple(start_ptr, end_ptr);
}

float master(long n, int nbprocess)
{
  int is_inital_req = 0;
  float final_result = 0.0;
  int req_id = -1;
  int work_sent = 0;
  int start, end = 0;
  float result = 0.0;

  for (int i = 1; i < nbprocess; i++)
  {
    MPI_Status *status;
    MPI_Request *request;
    request = new MPI_Request[3];
    status = new MPI_Status[3];
    for (int j = 0; j < 3; j++)
    {
      if (end < n)
      {
        req_id++;
        work_sent++;
        std::tie(start, end) = getData(req_id, n, nbprocess);
        int work[2] = {0};
        work[0] = start;
        work[1] = end;
        MPI_Isend(work, 2, MPI_INT, i, 0, MPI_COMM_WORLD, &request[j]);
      }
      else
      {
        MPI_Isend(0, 0, MPI_INT, i, QUIT, MPI_COMM_WORLD, &request[j]);
      }
    }
    MPI_Waitall(3, request, status);
  }

  while (work_sent != 0)
  {
    MPI_Status status;
    MPI_Recv(&result, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    int id = status.MPI_SOURCE;
    final_result += result;
    work_sent--;

    if (end < n)
    {
      req_id++;
      work_sent++;
      std::tie(start, end) = getData(req_id, n, nbprocess);
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

  return final_result;
}

void worker(int function_id, int intensity, float a, float b, long n)
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
      result = computeIntegral(start, end, function_id, intensity, a, b, n);
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
  int function_id, intensity;
  long int n;
  float a, b;
  float result = 0.0;
  
  MPI_Init(NULL, NULL);

  int nbprocess;
  MPI_Comm_size(MPI_COMM_WORLD, &nbprocess);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  sscanf(argv[1], "%i", &function_id);
  sscanf(argv[2], "%f", &a);
  sscanf(argv[3], "%f", &b);
  sscanf(argv[4], "%ld", &n);
  sscanf(argv[5], "%i", &intensity);

  using namespace std::chrono;
  high_resolution_clock::time_point start_time = high_resolution_clock::now();

  if (rank == 0)
  {
    result = master(n, nbprocess);
  }
  else
  {
    worker(function_id, intensity, a, b, n);
  }

  high_resolution_clock::time_point end_time = high_resolution_clock::now();
  duration<double> time_period = duration_cast<duration<double> >(end_time - start_time);
  if (rank == 0)
  {
    std::cout << result << std::endl;
    std::cerr << time_period.count() << std::endl;
  }

  MPI_Finalize();
  return 0;
}
