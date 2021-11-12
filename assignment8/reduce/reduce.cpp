#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <chrono>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int reduce(int* arr, unsigned long int n)
{
    int x;
    if (n <= 0)
        return 0;
    // if( n >= (1<<14))
    // {
      #pragma omp task shared(x)
      x = reduce_par(arr, n - 1) + arr[n - 1];
      #pragma omp taskwait
    // }
    // else
    // {
    //   x = reduce_par(arr, n - 1) + arr[n - 1];
    // }
    return x;
}


int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) {
    std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthread = atoi(argv[2]);
  int * arr = new int [n];
  float result = 0.0f;
  int sum = 0;

  omp_set_num_threads(nbthread);
  generateReduceData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      reduce(arr, n);
    }
  }
  
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  
  std::cout<<result<<std::endl;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
