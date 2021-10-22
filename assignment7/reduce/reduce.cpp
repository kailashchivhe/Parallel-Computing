#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


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
    
    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int *arr = new int [n];
    int nthreads = atoi(argv[2]);
    string kind = argv[3];
    int gran = atoi(argv[4]);
    int result=0;
    
    generateReduceData (arr, atoi(argv[1]));
    
    // insert reduction code here

    if (kind.compare("static")==0)
    {
        #pragma omp parallel for ordered schedule(static)
        for (int i = 0; i < n; i++) {
            sum=sum+ arr[i];
        } 
    }
    else if (kind.compare("dynamic")==0)
    {
        #pragma omp omp_set_schedule(dynamic)
        #pragma omp parallel for 
            for (int i = 0; i < n; i++) {
                sum = sum + arr[i];
            }
    }
    else if (kind.compare("guided")==0)
    {
        #pragma omp omp_set_schedule(guided)
        #pragma omp parallel for 
            for (int i = 0; i < n; i++) {
                sum = sum + arr[i];
            }
    }

    gettimeofday(&end, NULL);
    cout<<sum;
            
    double time = ((double)end.tv_sec- (double)start.tv_sec+( (double) end.tv_usec- (double)start.tv_usec)/ (double)1000000; // in seconds

    std::cerr<<time;

    delete[] arr;
    
    return 0;
}
