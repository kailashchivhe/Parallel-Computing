#include <iostream>
#include <ratio>
#include <chrono>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define ARR_START_SEND 100
#define ARR_START_RECV 100
#define ARR_END_SEND 101
#define ARR_END_RECV 101
#define RESULT_TAG 1000
#define MASTER 0

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

float (*function) (float,int);

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int a = atoi(argv[2]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  float multiplier = (atoi(argv[3]) - a) / (float)n;
  double integralPartial = 0.0;
  MPI_Comm comm;

  switch (atoi(argv[1]))
  {
  case 1:
    function = &f1;
    break;
  case 2:
    function = &f2;
    break;
  case 3:
    function = &f3;
    break;
  case 4:
    function = &f4;
    break;
  default:
    std::cerr << "Invalid function number provided\n";
    exit(-1);
  }

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double globalResult = 0.0;
  int master = 0;
  int chunkSize = n / (size);
  int arrIndex = 0;
  double receivedResult = 0.0;
  int startIndex, endIndex;
  MPI_Status status;

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  if (rank == 0)
  {
    int total_work_sent = 0, send_work[2], Received_status, final;
    for (int i = 1; i < size; i++)
    {
      send_work[0] = (i - 1) * chunkSize;
      send_work[1] = i * chunkSize;
      final = send_work[1];
      total_work_sent++;
      if (send_work[0] >= n)
      {
        send_work[0] = -1;
        send_work[1] = -1;
        total_work_sent--;
      }
      else
      {
        if (send_work[1] > n)
        {
          send_work[1] = n;
        }
      }
      MPI_Send(send_work, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    MPI_Status status;
    while (total_work_sent != 0)
    {
      MPI_Recv(&Received_status, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      total_work_sent--;
      globalResult = globalResult + Received_status;
      send_work[0] = final;
      send_work[1] = final + chunkSize;
      final = send_work[1];
      total_work_sent++;
      if (send_work[0] >= n)
      {
        send_work[0] = -1;
        send_work[1] = -1;
        total_work_sent--;
      }
      else
      {
        if (send_work[1] > n)
        {
          send_work[1] = n;
        }
      }
      MPI_Send(send_work, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
    }
  }
  else{
    int Received_work[2], Send_status;
    MPI_Recv(Received_work,2,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    while(Received_work[1]!=-1)
		{
			int localSum = 0;
			for(int x=Received_work[0];x<Received_work[1];x++)
			{
        localSum += (double)function(a + (x + 0.5) * x, intensity) * x;
			}
			Send_status = localSum;
			MPI_Send(&Send_status,1,MPI_INT,0,0,MPI_COMM_WORLD);
			MPI_Recv(Received_work,2,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
  }
  MPI_Finalize();
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  if (rank == 0)
  {
    std::cerr << elapsed_seconds.count() << std::endl;
    std::cout << globalResult << std::endl;
  }
  return 0;
}
