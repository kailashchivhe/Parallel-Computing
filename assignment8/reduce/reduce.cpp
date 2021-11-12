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

float findSum(int* begin, int* end)
{
    // base case
    // if (size == 0) {
    //     return 0;
    // }
    // else if (size == 1) {
    //     return arr[0];
    // }

    // int half = (int) std::floor(size / 2);
    // float x, y;

    // #pragma omp task shared(x) if(size-half >= (1<<14))
    // x = findSum(arr, half);
    
    // #pragma omp task shared(y) if(size-half >= (1<<14))
    // y = findSum(arr + half, size - half);
    
    // #pragma omp taskwait
    // x += y;

    // return x;

    size_t length = end - begin;
    size_t mid = length / 2;
    int sum = 0;

    if (length < 1000)
    {
      for (size_t ii = 1; ii < length; ii++)
      {
        begin[ii] += begin[ii - 1];
      }
    }
    else
    {
      #pragma omp task shared(sum)
      {
        sum = recursiveSumBody(begin, begin + mid);
      }
      #pragma omp task
      {
        recursiveSumBody(begin + mid, end);
      }
      #pragma omp taskwait

      #pragma omp parallel for
      for (size_t ii = mid; ii < length; ii++)
      {
        begin[ii] += sum;
      }
    }
    return begin[length - 1];
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
    result = findSum(&arr[0], &arr[n]);
  }
  
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  
  std::cout<<result<<std::endl;

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
