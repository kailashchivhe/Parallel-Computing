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

float global_result = 0, global_x_int;
float a, b;
unsigned long n;
double cpu_time;
int func, intensity, granularity, work_done = 0, nbthreads;
char* sync;
unsigned long startloop=0, endloop;
float result = 0, global_x_val ;

pthread_mutex_t loop_locks, global_result_lock, iteration_lock;

struct arguments
{
    float a, b;
    unsigned long n, start, end;
    float result, x_val=0, x_int;
    int intensity, func;
};

int get_end(int start)
{	
	int endloop = (start + granularity);
	if( endloop >= n - 1)
		return n;
	return endloop;
}

int get_start()
{
	int temp;
	pthread_mutex_lock(&loop_locks);
	temp = startloop;
	startloop = startloop + granularity;
	if (startloop + granularity > n)
		work_done = 1;
	pthread_mutex_unlock(&loop_locks);
	return temp;
}

//This function does integration using chunk level mutual exclusion. The critical section is in the while loop for every computing thread.

void* integrate_chunk_level(void *unused)
{
	float chunk_result = 0, chunk_int, chunk_val=0;
	unsigned long loop_end, loop_start;
	while(work_done != 1)
	{
		loop_start = get_start();
		loop_end = get_end(loop_start);
		for(int i = loop_start; i < loop_end; i++)
    {	
			chunk_int = (a + (i + 0.5) * ((b - a) / (float)n));
			chunk_val = chunk_val + chunk_int;
			switch(func)
        	{
      			case 1: chunk_result = f1(chunk_val, intensity) * ((b - a)/n);
						break;
        		case 2: chunk_result = f2(chunk_val, intensity) * ((b - a)/n);
						break;
        	  	case 3: chunk_result = f3(chunk_val, intensity) * ((b - a)/n);
						break;
      		  	case 4: chunk_result = f4(chunk_val, intensity) * ((b - a)/n);
						break;
        	  	default: std::cout<<"\nWrong function id"<<std::endl;
      		}
    	  	
		}	
		pthread_mutex_lock(&global_result_lock);
		if ( endloop>=n-1)
    	  		work_done = 1;
    	pthread_mutex_unlock(&global_result_lock);
	}
	pthread_mutex_lock(&global_result_lock);
    global_result = global_result + chunk_result;
    pthread_mutex_unlock(&global_result_lock);
}

//This function does integration using thread level mutual exclusion where the every thread computes the result in the local variable and aggregates the result in the end.

void* integrate_thread_level(void * argument)
{
    struct arguments* arg = (struct arguments* )argument;
    
    while(work_done!=1)
    {
    
    	arg->start = get_start();
	if (arg->start >= n)
		break;
    	arg->end = get_end(arg->start);
   
    	for(int i = arg->start; i < arg->end; i++)
    	{
    	  
    		arg->x_int = (arg->a + (i + 0.5) * ((arg->b - arg->a) / (float)arg->n));
			arg->x_val = arg->x_val + arg->x_int;
			switch(arg->func)
    		{
      			case 1: arg->result = f1(arg->x_val, arg->intensity) * ((arg->b - arg->a)/arg->n);
	      		break;
          		case 2: arg->result = f2(arg->x_val, arg->intensity) * ((arg->b - arg->a)/arg->n);
	      		break;
          		case 3: arg->result = f3(arg->x_val, arg->intensity) * ((arg->b - arg->a)/arg->n);
	      		break;
      	  		case 4: arg->result = f4(arg->x_val, arg->intensity) * ((arg->b - arg->a)/arg->n);
	      		break;
            	default: std::cout<<"\nWrong function id"<<std::endl;
      	 	}
    	}
    pthread_mutex_lock(&global_result_lock);
		if (endloop>=n-1)
    	  		work_done = 1;
    pthread_mutex_unlock(&global_result_lock);
    }
    pthread_exit(NULL);
}

int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  func = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atof(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  sync = argv[7];
	granularity = atoi(argv[8]);
  pthread_t *threads;
	struct arguments* arg;
	
  threads = (pthread_t *)malloc(nbthreads * sizeof(pthread_t));
  arg = (struct arguments*)malloc(nbthreads * sizeof(arguments));
    
  std::chrono::time_point<std::chrono::system_clock> clock_start = std::chrono::system_clock::now();
    
    
  if ( strcmp(sync, "thread") == 0)
  {
    for ( int j = 0; j < nbthreads; j++)
    {
    	arg[j].a = a;
    	arg[j].b = b;
    	arg[j].intensity =intensity;
    	arg[j].func = func;
    	arg[j].n = n;
    	pthread_create(&threads[j], NULL, integrate_thread_level, (void *)&(arg[j])); 
    }
	  for ( int i = 0; i < nbthreads; i++)
    {
    	pthread_join(threads[i], NULL);
    }
    for( int k = 0; k < nbthreads; k++)
    {
	    pthread_mutex_lock(&global_result_lock);
    	global_result += arg[k].result;
	    pthread_mutex_unlock(&global_result_lock);
    }
  }
  else if( strcmp(sync, "chunk") == 0)
 	{
 		for(int i = 0; i < nbthreads; i++)
 		{
 			pthread_create(&threads[i], NULL, integrate_chunk_level, NULL);
 		}
 	
 	  for(int i = 0; i < nbthreads; i++)
 		{
 			pthread_join(threads[i], NULL);
 		}
 	}
 	
 	std::chrono::time_point<std::chrono::system_clock> clock_end = std::chrono::system_clock::now();
  std::chrono::duration<double>diff = clock_end - clock_start;
  std::cout<<global_result;
  std::cerr<<diff.count();
  return 0;
}
