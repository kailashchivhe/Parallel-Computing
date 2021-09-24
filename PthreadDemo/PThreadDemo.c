#include <stdio.h>
#include <pthread.h>

// gcc -pthread OR g++ -pthread
void *f( void *p ){
    printf ("ITCS5081 Parallel Computing %d \n", p );
    return NULL ;
}

int main () {
    pthread_t tasks[5];
    
    // create 5 threads
    for (int i =0; i < 5; ++ i )
    {
        pthread_create( & tasks [i], NULL , f , i );
    }
    
    // wait for the 5 threads to complete
    for (int i =0; i < 5; ++ i )
    {
        pthread_join ( tasks [ i], NULL );
    }
    
    return 0;
}