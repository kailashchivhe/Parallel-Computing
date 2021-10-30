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

void merge(int arr[], int tempArray[], int start, int mid, int end, int size)
{
  int k = start, i = start, j = mid + 1;

  while (i <= mid && j <= end)
  {
    if (arr[i] < arr[j])
    {
      tempArray[k++] = arr[i++];
    }
    else
    {
      tempArray[k++] = arr[j++];
    }
  }

  while (i < size && i <= mid)
  {
    tempArray[k++] = arr[i++];
  }

  for (int i = start; i <= end; i++)
  {
    arr[i] = tempArray[i];
  }
}

void mergesort(int arr[], int tempArray[], int start, int end, int n, int nbthreads)
{
  omp_set_num_threads(nbthreads);

  omp_set_schedule(omp_sched_dynamic, 2);

  for(int bSize = 1; bSize < end - start; bSize = 2 * bSize)
  {
    #pragma omp parallel for schedule(runtime)
    for (int i = start; i <= end; i = i + 2 * bSize)
    {
      int front = i;
      int mid = i + bSize - 1;
      int rear = min(i + 2 * bSize - 1, end);

      merge(arr, tempArray, front, mid, rear, n);
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

  int size = atoi(argv[1]);
  int nbthread = atoi(argv[2]);

  int startIndex = 0;
  int endIndex = size-1;
  
  // get arr data
  int * arr = new int [size];

  generateMergeSortData (arr, size);
  
  int tempArray[size];
  
  for (int i = 0; i < size; i++) 
  {
    tempArray[i] = arr[i];
  }

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
  
  mergesort(arr, tempArray, startIndex, endIndex, size, nbthread);

  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;

  checkMergeSortResult(arr, size);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
