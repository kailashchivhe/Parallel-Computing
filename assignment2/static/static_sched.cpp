#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

pthread_mutex_t mut;

float sum = 0;
struct Parameters{
    int start;
    int end;
    int fnId;
    float a;
    float b;
    float n;
    int intensity;
    int nbthreads;
};

void* static_worker_thread(void *value){
    struct Parameters *th = (struct Parameters*) value;
    pthread_mutex_lock(&mut);
    for(int i=th->start;i<th->end;i++){
        float x = (th->a + (i + 0.5) * ((th->b-th->a)/th->n));
        if(th->fnId == 1){
            sum += (f1(x,th->intensity)*((th->b-th->a)/th->n));
        }else if(th->fnId == 2){
            sum += (f2(x,th->intensity)*((th->b-th->a)/th->n));
        }else if(th->fnId == 3){
            sum += (f3(x,th->intensity)*((th->b-th->a)/th->n));
        }else if(th->fnId == 4){
            sum += (f4(x,th->intensity)*((th->b-th->a)/th->n));
        }
        pthread_mutex_unlock(&mut);
    }
    pthread_exit(NULL);
}


void *static_worker_iter(void *value){
    struct Parameters *th =(struct Parameters *)value;
    for(int i=th->start; i<th->end; i++){
        pthread_mutex_lock(&mut);
            float x = (th->a + (i + 0.5) * ((th->b-th->a)/th->n));
            if(th->fnId == 1){
                sum += (f1(x,th->intensity)*((th->b-th->a)/th->n));
            }else if(th->fnId == 2){
                sum += (f2(x,th->intensity)*((th->b-th->a)/th->n));
            }else if(th->fnId == 3){
                sum += (f3(x,th->intensity)*((th->b-th->a)/th->n));
            }else if(th->fnId == 4){
                sum += (f4(x,th->intensity)*((th->b-th->a)/th->n));
            }
        pthread_mutex_unlock(&mut);
    }
    pthread_exit(NULL);
}

int main (int argc, char* argv[]) {
    if (argc < 8) {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
        return -1;
    }
    
    float fnId= atoi(argv[1]);
    float a = atoi(argv[2]);
    float b = atoi(argv[3]);
    float n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    int nbthreads = atoi(argv[6]);

    int start=0;
    int end=0;

    pthread_t *threads;
    threads = new pthread_t[nbthreads];

    //instance of a struct
    struct Parameters *strt;
    strt = new struct Parameters[nbthreads];
    
    pthread_mutex_init(&mut, NULL);
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    
    //calculate your result here
    
    for(int i=0;i<nbthreads;i++){
        strt[i].fnId= fnId;
        strt[i].a = a;
        strt[i].b = b;
        strt[i].n = (float)n;
        strt[i].intensity = intensity;
        strt[i].start = i * (n/nbthreads);
        strt[i].end = (i+1) * (n/nbthreads);
        if(strcmp(argv[7],"iteration") == 0 )
        {
            pthread_create(&threads[i],NULL,static_worker_iter,(void*) &strt[i]);
        }
        else
        {
            pthread_create(&threads[i],NULL,static_worker_thread,(void*) &strt[i]);
        }
    }
    // loop to join threads
    for(int i = 0;(i <nbthreads); i++){
        pthread_join(threads[i], NULL);
    }
    
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    std::cout<<sum<<std::endl;

    std::cerr<<elapsed_seconds.count()<<std::endl;

    pthread_mutex_destroy(&mut);
    
    return 0;
}
