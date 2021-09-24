#include <stdio.h>
#include <pthread.h>

void *comp(void *px){
  int* v = (int *)px;
  printf("comp: %d\n", (v) );
  v += 1;
  printf("next comp: %d\n", (v) );
  return NULL;
}

int* count_numbers(char *str, int n){
  int counts[10];
  for (int I=0; I<10; I++){
    counts[I]=0;
  }
  int nbthreads=10;
  pthread_t th[nbthreads];
  
  for (int I=0; I<nbthreads; ++I){
    pthread_create(&th[I], NULL, comp, &counts[I]);
  }

    // for (int k =0; k < nbthreads; ++ k )
    // {
    //     pthread_join ( th [ k], NULL );
    // }
  return &(counts[0]);
}

// pthread_mutex_t mut;

// // gcc -pthread OR g++ -pthread
// void *f( void *p ){
//     int* val = (int*) p;
//     for( int i=0;i<100000;++i){
//         pthread_mutex_lock(&mut);
//         *val += 1;
//         pthread_mutex_unlock(&mut);
//     }
//     // printf ("ITCS5081 Parallel Computing %d \n", p );
//     return NULL ;
// }

int main () {
    // pthread_t tasks[50];
    // int val = 0;

    // pthread_mutex_init( &mut, NULL );
    // // create 5 threads
    // for (int i =0; i < 50; ++ i )
    // {
    //     pthread_create( & tasks [i], NULL , f , &val );
    // }
    
    // // wait for the 5 threads to complete
    // for (int i =0; i < 50; ++ i )
    // {
    //     pthread_join ( tasks [ i], NULL );
    // }
    
    // pthread_mutex_destroy(&mut);

    // printf( " %d\n", val );
    int* arr = count_numbers("kailash",7);
    for(int i=0;i<7;i++)
        printf("main: %d\n",*(arr+i));
    return 0;
}