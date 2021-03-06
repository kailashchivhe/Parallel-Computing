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
  int result = 0;
  int start, end;

  omp_set_num_threads(nbthread);
  generateReduceData (arr, n);
  int gran = n / nbthread; 
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  #pragma omp parallel 
  {
    #pragma omp single
    for(int i=0; i<n; i+=gran){
      int j,start=i, end=i+gran-1;
      if(end > n){
        end = n-1;
      }
      #pragma omp task 
      { int p=0;
        for(j=start;j<=end;j++)
        {
          p += arr[j];
        }
        #pragma omp critical
        result += p;
      }
    }
  }
  
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  
  std::cout<<result<<std::endl;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
