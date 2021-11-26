#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

double calc_numerical_integration(int functionid, float a, float b, int n, int intensity, int start, int end )
{
    float x;
    float t1 = (b - a) / n;
    double sum = 0.0L;
    for( int i=start ; i<end ; i++ )
    {
        x = a + ((i + 0.5) * t1);
        switch(function_id)
        {
            case 1: sum += f1(x,intensity);
                break;
            case 2: sum += f2(x,intensity);
                break;
            case 3: sum += f3(x,intensity);
                break;
            case 4: sum += f4(x,intensity);
                break;
            default: return -1;
        }
    }
    return (t1*sum);
}

int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  int n, process, function_id, intensity, rank, elements_per_process, n_chunk_received, start_received;
  float my_result, a, b,result;
  MPI_Status status;
  
  function_id = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);

  double starttime, endtime;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process);

  starttime = MPI_Wtime();

  if (rank == 0)
  {
    int chunk = n/process;
    int index,i;
    result = my_result;
    for (i = 1; i < process-1; i++)
    {
      index = i * elements_per_process;
      MPI_Send(&chunk, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(&index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    index = i * elements_per_process;
    int elements_left = n - index;
    MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    MPI_Send(&index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

    float sum = 0;
    sum = calc_numerical_integration(function_id, a, b, n, intensity, 0, chunk );
    float tmp;
    for (i = 1; i < process; i++)
    {
      MPI_Recv(&tmp, 1, MPI_INT,
               MPI_ANY_SOURCE, 0,
               MPI_COMM_WORLD,
               &status);
      int sender = status.MPI_SOURCE;
      sum += tmp;
    }

    std::cout<<sum;  
  }
  else
  {
    MPI_Recv(&n_chunk_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    MPI_Recv(&start_received, n_elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    float partial_sum = 0;

    partial_sum = calc_numerical_integration(function_id, a, b, n, intensity, start_received, n_chunk_received );

    MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  
  endtime = MPI_Wtime();
  
  MPI_Finalize();

  std::cerr<<endtime-starttime<<std::endl;

  return 0;
}
