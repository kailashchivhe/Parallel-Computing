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

#define CUTOFF 100

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

float serial_sum(int *a, size_t n)
{
    // base cases
    if (n == 0) {
        return 0.;
    }
    else if (n == 1) {
        return a[0];
    }

    // recursive case
    size_t half = n / 2;
    return serial_sum(a, half) + serial_sum(a + half, n - half);
}

float findSum(int* arr, int size)
{
    // base case
    if (size <= CUTOFF) {
        return serial_sum(arr, size);
    }

    // recursive case
    size_t half = size / 2;
    float x, y;

    #pragma omp task shared(x)
    x = findSum(arr, half);
    
    #pragma omp task shared(y)
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
  int total = 0;

  omp_set_num_threads(nbthread);
  generateReduceData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

  // #pragma omp parallel
  // #pragma omp single nowait
  // result = findSum(arr, n);

  #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp taskgroup task_reduction(+: total)
            {
                for(int i = 0; i < ARRAY_SIZE; i++)
                {
                    #pragma omp task in_reduction(+: total)
                    total += myArray[i];
                }
            }
        }
    }
  
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  
  std::cout<<total<<std::endl;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
