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
#include <chrono>

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
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

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
    
    generateReduceData (arr, n);
    // insert reduction code here
    if (kind.compare("static")==0)
    {
        #pragma omp parallel for schedule(static) num_threads(nthreads) reduction(+:sum) 
	    for (int i = 0; i < n; i++) {
	        sum += arr[i];
        } 
    }
    else if (kind.compare("dynamic")==0)
    {
        #pragma omp parallel for schedule(dynamic,gran) num_threads(nthreads) reduction(+:sum) 
	    for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    }
    else if (kind.compare("guided")==0)
    {
        #pragma omp parallel for schedule(guided,gran) num_threads(nthreads) reduction(+:sum) 
	    for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapased_seconds = end-start;
    cout<<sum;
    std::cerr<<elapased_seconds.count()<<std::endl;
    delete[] arr;
    return 0;
}
