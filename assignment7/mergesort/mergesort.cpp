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

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    int leftArray[n1], rightArray[n2];
 
    for (i = 0; i < n1; i++)
    {
        leftArray[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++)
    {
        rightArray[j] = arr[m + 1+ j];
    }
 
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (leftArray[i] <= rightArray[j])
        {
            arr[k] = leftArray[i];
            i++;
        }
        else
        {
            arr[k] = rightArray[j];
            j++;
        }
        k++;
    }
 
    while (i < n1)
    {
        arr[k] = leftArray[i];
        i++;
        k++;
    }
 
    while (j < n2)
    {
        arr[k] = rightArray[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int n, int nthreads)
{
  #pragma omp parallel for schedule(runtime) num_threads(nthreads)
   for (int i=1; i<=n-1; i = 2*i)
   {
       for (int j=0; j<n-1; j += 2*i)
       {
           int mid = min(j + i - 1, n-1);
           int right_end = min(j + 2*i - 1, n-1);
           merge(arr, j, mid, right_end);
       }
   }
}

int main(int argc, char *argv[])
{

  //forces openmp to create the threads beforehand
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
  int nbthread = atoi(argv[2]);

  omp_set_num_threads(nbthread);

  omp_set_schedule( omp_sched_dynamic, -1 );

  // get arr data
  int *arr = new int[n];
  int *tmp = new int[n];

  generateMergeSortData(arr, n);

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  mergeSort( arr, n, nbthread );

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  checkMergeSortResult(arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;
  delete[] tmp;

  return 0;
}
