#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
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
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int nbthread = atoi(argv[2]);

  int * arr = new int [n];

  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  //insert prefix sum code here
  
  omp_set_num_threads(nbthread);

  omp_set_schedule(omp_sched_dynamic,-1);
  
  int *tarr = new int[nbthread];
  
  pr[0] = 0;

  //insert prefix sum code here

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    int total = 0;

    #pragma omp for schedule(runtime)
    for (int i = 0; i < n; i++)
    {
      total += arr[i];
      pr[i + 1] = total;
    }

    #pragma omp critical
    tarr[id] = total;

    #pragma omp barrier
    int sum = 0;
    for (int j = 0; j < id; j++)
    {
      sum += tarr[j];
    }

    #pragma omp for schedule(runtime)
    for (int k = 0; k < n; k++)
    {
      pr[k + 1] += sum;
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = end-start;
  
  checkPrefixSumResult(pr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
