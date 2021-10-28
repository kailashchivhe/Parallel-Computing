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

void merge(int *arr, int start, int mid, int end )
{
  int temp[end - start + 1];

	int i = start, j = mid+1, k = 0;

	while(i <= mid && j <= end) {
		if(arr[i] <= arr[j]) {
			temp[k] = arr[i];
			k += 1; i += 1;
		}
		else {
			temp[k] = arr[j];
			k += 1; j += 1;
		}
	}

	while(i <= mid) {
		temp[k] = arr[i];
		k += 1; i += 1;
	}

	while(j <= end) {
		temp[k] = arr[j];
		k += 1; j += 1;
	}

	for(i = start; i <= end; i += 1) {
		arr[i] = temp[i - start];
	}
}

void mergesort(int * arr, int begin, int end)
{
   if ( begin >= end ) 
   {
      return;
   }

   int mid = (begin + end) / 2;

   #pragma omp task firstprivate( arr, begin, mid )
   mergesort( arr, begin, mid );

   #pragma omp task firstprivate( arr, mid, end )
   mergesort( arr, mid+1, end );
	
   #pragma omp taskwait
   merge( arr, begin, mid, end );
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

  omp_set_schedule( omp_sched_static,-1 );

  // get arr data
  int *arr = new int[n];
  int *tmp = new int[n];

  generateMergeSortData(arr, n);

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  #pragma omp parallel
  {
    #pragma omp single
    mergesort(arr, 0, n-1 );
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  checkMergeSortResult(arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;
  delete[] tmp;

  return 0;
}
