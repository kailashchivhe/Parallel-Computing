#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <chrono>

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

void mpi_numint(int f, float a, float b, int intensity, int n, int myrank, int mysize)
{
  float pre_product = (b - a) / n;
  float function_eval;
  int N = n;
  int P = mysize;
  float mysummed_value = 0;
  float fullsum;
  for (int i = (myrank)*N / P; i < (myrank + 1) * N / P; i++)
  {
    switch (f)
    {
    case 1:
      mysummed_value += f1(a + (i + 0.5) * pre_product, intensity);
      break;
    case 2:
      mysummed_value += f2(a + (i + 0.5) * pre_product, intensity);
      break;
    case 3:
      mysummed_value += f3(a + (i + 0.5) * pre_product, intensity);
      break;
    case 4:
      mysummed_value += f4(a + (i + 0.5) * pre_product, intensity);
      break;
    }
  }
  mysummed_value = pre_product * (mysummed_value);
  MPI_Reduce(&mysummed_value, &fullsum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (myrank == 0)
    cout << fullsum;
}

int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return 0;
  }

  int funcid = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  int myrank,mysize;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &mysize);
  std::chrono::time_point<std::chrono::system_clock> start; 
  
  if(myrank ==0)
      start = std::chrono::system_clock::now();

  mpi_numint(funcid,a,b,intensity,n,myrank,mysize);


  MPI_Finalize();
  if(myrank==0)
  {
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;

  std::cerr<<elapsed_seconds.count()<<std::endl;
  }
  


  return 0;
}
