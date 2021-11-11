#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void merge(int * arr, int l, int mid, int r) {
  // short circuits
  if (l == r) 
  {
    return;
  }

  if (r-l == 1) 
  {
    if (arr[l] > arr[r]) {
      int temp = arr[l];
      arr[l] = arr[r];
      arr[r] = temp;
    }
    return;
  }

  int i, j, k;
  int n = mid - l;
  
  // declare and init temp arrays
  int *temp = new int[n];
  for (i=0; i<n; ++i)
    temp[i] = arr[l+i];

  i = 0;    // temp left half
  j = mid;  // right half
  k = l;    // write to 

  // merge
  while (i<n && j<=r) {
     if (temp[i] <= arr[j] ) {
       arr[k++] = temp[i++];
     } else {
       arr[k++] = arr[j++];
     }
  }

  // exhaust temp 
  while (i<n) {
    arr[k++] = temp[i++];
  }

  // de-allocate structs used
  delete[] temp;
}


void mergesort(int * arr, int l, int r) 
{
  if (l < r) {
    int mid = (l+r)/2;
    #pragma omp task
    mergesort(arr, l, mid);

    #pragma omp task
    mergesort(arr, mid+1, r);

    #pragma omp taskwait
    merge(arr, l, mid+1, r);
  }
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
  
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthread = atoi(argv[2]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  omp_set_num_threads(nbthread);

  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
  //insert sorting code here.
  #pragma omp parallel
   {
      #pragma omp single
      mergesort(arr, 0, n-1);
   }
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = endTime-startTime;
  checkMergeSortResult (arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
