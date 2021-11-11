#include <stdio.h>
#include <pthread.h>

// gcc -pthread OR g++ -pthread
void *f( void *p ){
    int* i = (int*) p;
    printf ("ITCS5081 Parallel Computing %d \n", *i );
    return NULL ;
}

int main () {
    pthread_t tasks[5];
    int arr[5];
    // create 5 threads
    for (int i =0; i < 5; ++ i )
    {
        arr[i] = i;
        pthread_create( & tasks [i], NULL , f , &arr[i] );
    }
    
    // wait for the 5 threads to complete
    for (int i =0; i < 5; ++ i )
    {
        pthread_join ( tasks [ i], NULL );
    }
    
    return 0;
}