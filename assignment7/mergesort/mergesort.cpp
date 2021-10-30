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

void merge(int *arr, int l, int m, int r)
{
  int i, j, k;
  int nleft = m - l + 1;
  int nright = r - m;
  int *temp_left = new int[nleft];
  int *temp_right = new int[nright];

  for (i = 0; i < nleft; i++)
    temp_left[i] = arr[l + i];

  for (j = 0; j < nright; j++)
    temp_right[j] = arr[m + 1 + j];

  i = 0;
  j = 0;
  k = l;

  while (i < nleft && j < nright)
  {
    if (temp_left[i] <= temp_right[j])
    {
      arr[k] = temp_left[i];
      i++;
    }
    else
    {
      arr[k] = temp_right[j];
      j++;
    }
    k++;
  }

  while (i < nleft)
  {
    arr[k] = temp_left[i];
    i++;
    k++;
  }

  while (j < nright)
  {
    arr[k] = temp_right[j];
    j++;
    k++;
  }

  delete[] temp_left;
  delete[] temp_right;
}

void mergeSort(int *arr, int l, int r, int nbthreads)
{
  omp_set_num_threads(nbthreads);
  int n = r;
  for (int k = 1; k < n + 1; k *= 2)
  {
    #pragma omp parallel for schedule(static,1)

    for (int i = 0; i < n + 1; i += (2 * k))
    {
      int left = i;
      int mid = i + (k - 1);
      int right = i + ((2 * k) - 1);
      if (mid >= n)
      {
        mid = (i + n - 1) / 2;
        right = n - 1;
      }
      else if (right >= n)
      {
        right = n - 1;
      }
      merge(arr, left, mid, right);
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
  int nbthread = atoi(argv[2]);

  // get arr data
  int *arr = new int[n];

  generateMergeSortData(arr, n);

  //insert sorting code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  mergeSort(arr, 0, n-1, nbthread);

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  checkMergeSortResult(arr, n);

  std::cerr<<elapsed_seconds.count()<<std::endl;

  delete[] arr;

  return 0;
}
