#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

using namespace std;
int sum = 0;

#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
    //forces openmp to create the threads beforehand
    struct timeval start, end;
    gettimeofday(&start, NULL);
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
    
    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int *arr = new int [n];
    int nthreads = atoi(argv[2]);
    string kind = argv[3];
    int gran = atoi(argv[4]);
    if( gran < 0 ){
        gran = 1;
    }
    int result=0;
    
    generateReduceData (arr, n);
    
    // insert reduction code here

    if (kind.compare("static")==0)
    {
        #pragma omp parallel num_threads(nthreads) schedule(static,gran) reduction(+:sum)
        {
            for (int i = 0; i < n; i++) {
                sum=sum+ arr[i];
            }
        } 
    }
    else if (kind.compare("dynamic")==0)
    {
        #pragma omp parallel num_threads(nthreads) schedule(dynamic,gran) reduction(+:sum)
        {
            for (int i = 0; i < n; i++) {
                sum = sum + arr[i];
            }
        }
    }
    else if (kind.compare("guided")==0)
    {
        #pragma omp parallel num_threads(nthreads) schedule(guided,gran) reduction(+:sum) 
        {
            for (int i = 0; i < n; i++) {
                sum = sum + arr[i];
            }
        }
    }

    gettimeofday(&end, NULL);
    cout<<sum;
            
    double time = ((double)end.tv_sec-(double)start.tv_sec+((double)end.tv_usec-(double)start.tv_usec)/(double)1000000);

    std::cerr<<time;

    delete[] arr;
    
    return 0;
}
