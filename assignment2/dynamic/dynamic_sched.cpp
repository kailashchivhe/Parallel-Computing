#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

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

//chunk
const char chu = 'c';
//thread
const char thr = 't';

float (*pointerFunc)(float, int);
pthread_mutex_t mutOne, mutTwo;

struct threadLooping
{
    
    int startGran, endGran, chunk_size, n, intenTT;
    float a, b, rResult;
    float* result;
    bool finish;
    int *prog;
    char sync;
    
    
    
    bool done()
    {
        
        if (*prog < endGran)
        {
            finish = false;
        }
        else
        {
            finish = true;
        }
        
        return finish;
    }
    
    void get_next(int *begin, int *end)
    {
        
        
        pthread_mutex_lock(&mutTwo);
        
        if (*prog == 0)
        {
            *begin = startGran;
        }
        else
        {
            *begin = *prog;
        }
        
        if (*begin + chunk_size <= endGran)
        {
            *end = *begin + chunk_size;
        }
        else
        {
            *end = endGran;
        }
        
        *prog = *end;
        
        pthread_mutex_unlock(&mutTwo);
    }
    
};

float integrationFunc(int i, int n, int intenTT, float a, float b)
{
    return pointerFunc((a + (i + 0.5) * ((b - a)/n)), intenTT) * ((b - a)/n);
}

void* threadFunction(void *arg)
{
    threadLooping *th = (threadLooping*)arg;
    float *res = th->result;
    int begin = 0, end = 0;
    
    while(!(th->done()))
    {
        
        th->get_next(&begin, &end);
        
        float tmp = 0;
        
        for(int i = begin; i < end; i++)
        {
            if(th->sync == thr)
            {
                th->rResult += integrationFunc(i, th->n, th->intenTT, th->a, th->b);
            }
            else if (th->sync == chu)
            {
                tmp += integrationFunc(i, th->n, th->intenTT, th->a, th->b);
            }
        }
        
        if (th->sync == chu)
        {
            pthread_mutex_lock(&mutOne);
            *res += tmp;
            pthread_mutex_unlock(&mutOne);
        }
        
    }
}

int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  std::chrono::time_point<std::chrono::system_clock> start_time, end_time;
    
    int functionid = atoi(argv[1]);
    float a = atof(argv[2]);
    float b = atof(argv[3]);
    int n = atoi(argv[4]);
    int intenTT = atoi(argv[5]);
    int nbthreads = atoi(argv[6]);
    char sync = *argv[7];
    int chunk = atoi(argv[8]);
    
    int prog = 0;
    
    
    switch(functionid)
    {
        case 1: pointerFunc = &f1;
            break;
        case 2: pointerFunc = &f2;
            break;
        case 3: pointerFunc = &f3;
            break;
        case 4: pointerFunc = &f4;
            break;
        default: return -1;
    }
    float totalSum = 0;
    std::vector<pthread_t> parti(nbthreads);
    std::vector<threadLooping> loopN(nbthreads);
    int start = 0;
    
    start_time = std::chrono::system_clock::now();
    
    for(int i = 0; i < nbthreads; i++)
    {
        loopN[i].result = &totalSum;
        loopN[i].rResult = 0;
        loopN[i].a = a;
        loopN[i].b = b;
        loopN[i].n = n;
        loopN[i].intenTT = intenTT;
        loopN[i].startGran = start;
        loopN[i].endGran = n;
        loopN[i].chunk_size = chunk;
        loopN[i].sync = sync;
        loopN[i].prog = &prog;
        loopN[i].finish = false;
        pthread_create(&parti[i], NULL , threadFunction , &loopN[i]);
    }
    
    for(int i = 0; i < nbthreads; i ++)
    {
        pthread_join(parti[i], NULL);
    }
    
    if(sync == thr)
    {
        for(int i = 0; i < nbthreads; i++)
        {
            totalSum += loopN[i].rResult;
        }
    }
    
    end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> time_elapsed = end_time - start_time;
    
    std::cout << totalSum << std::endl;
    std::cerr << time_elapsed.count() << std::endl;

  return 0;
}
