#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct data{
    int tid;
    int total_threads;
};

pthread_mutex_t mut;

void* print(void *ptr){
    struct data *local = (struct data *) ptr;
    int t_id = local->tid;
    int total_threads = local->total_threads;
    pthread_mutex_lock(&mut);
    cout << "I am thread " << t_id << " in " << total_threads << endl;
    pthread_mutex_unlock(&mut);
    return NULL;
}

int main (int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
        return -1;
    }
    
    int total_threads = atoi(argv[1]);
    pthread_mutex_init(&mut, NULL);
    
    pthread_t* threads = (pthread_t*)(new pthread_t[total_threads]);
    struct data d[total_threads];
    
    for (int i = 0; i < total_threads; i++){
        d[i].tid = i;
        d[i].total_threads = total_threads;
        pthread_create(&threads[i], NULL, print, (void *)&d[i]);
    }
    
    for (int i = 0; i < total_threads; i++){
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
