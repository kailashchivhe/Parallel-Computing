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


int main (int argc, char* argv[]) {

    #pragma omp parallel num_threads(5){
        cout<<"Hello World !"<<omp_get_thread_num();
    }

}