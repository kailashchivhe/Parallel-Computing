#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <chrono>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void merge(int arr[], int tmp[], int startPosition, int middle, int endPosition, int n)
{

  int k = startPosition, i = startPosition, j = middle + 1;

  while (i <= middle && j <= endPosition)
  {
    if (arr[i] < arr[j])
    {
      tmp[k++] = arr[i++];
    }
    else
    {
      tmp[k++] = arr[j++];
    }
  }

  while (i < n && i <= middle)
  {
    tmp[k++] = arr[i++];
  }

  for (int i = startPosition; i <= endPosition; i++)
  {
    arr[i] = tmp[i];
  }
}

void mergesort(int arr[], int tmp[], int start, int end, int n, int nbthreads)
{
  omp_set_num_threads(nbthreads);
  omp_set_schedule(omp_sched_static, 1);

  for (int blockSize = 1; blockSize < end - start; blockSize = 2 * blockSize)
  {
    #pragma omp parallel for schedule(runtime)
    for (int i = start; i <= end; i = i + 2 * blockSize)
    {
      int startPosition = i;
      int middle = i + blockSize - 1;
      int endPosition = min(i + 2 * blockSize - 1, end);

      merge(arr, tmp, startPosition, middle, endPosition, n);
    }
  }
}

int main(int argc, char *argv[])
{
  #pragma omp parallel
  {
    int fd = open(argv[0], O_RDONLY);
    if (fd != -1)
    {
      close(fd);
    }
    else
    {
      std::cerr << "something is amiss" << std::endl;
    }
  }

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " <n> <nbthreads>" << std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);

  int start = 0;
  int end = n-1;
  
  // get arr data
  int * arr = new int [n];

  generateMergeSortData (arr, n);
  
  int tmp[n];
  
  for (int i = 0; i < n; i++) 
  {
    tmp[i] = arr[i];
  }

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
  
  mergesort(arr, tmp, start, end, n, nbthreads);

  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;

  checkMergeSortResult(arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
