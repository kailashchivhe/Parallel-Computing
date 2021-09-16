#include <stdio.h>
#include <pthread.h>

// gcc -pthread OR g++ -pthread
void *f( void *p ){
    printf ("ITCS5081 Parallel Computing a b c \n" );
    return NULL ;
}

int main () {
    pthread_t tasks[5];
    
    // create 5 threads
    for (int i =0; i < 5; ++ i )
    {
        pthread_create( & tasks [i], NULL , f , NULL );
    }
    
    // wait for the 5 threads to complete
    for (int i =0; i < 5; ++ i )
    {
        pthread_join ( tasks [ i], NULL );
    }
    
    return 0;
}