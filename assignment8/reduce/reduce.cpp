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

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

float findSum(int* arr, int size)
{
    // base case
    if (size == 0) {
        return 0;
    }
    else if (size == 1) {
        return arr[0];
    }

    int half = (int) std::floor(size / 2);
    float x, y;

    #pragma omp task shared(x) if(size-half >= (1<<14))
    x = findSum(arr, half);
    
    #pragma omp task shared(y) if(size-half >= (1<<14))
    y = findSum(arr + half, size - half);
    
    #pragma omp taskwait
    x += y;

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

  omp_set_num_threads(nbthread);
  generateReduceData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  #pragma omp parallel
  {
    #pragma omp single
    result = findSum(arr, n);
  }
  
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  
  std::cout<<result<<std::endl;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
