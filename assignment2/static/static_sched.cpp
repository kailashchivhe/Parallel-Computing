#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <chrono>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

  float f1(float x, int intensity);
  float f2(float x, int intensity);
  float f3(float x, int intensity);
  float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float global_result = 0;
float global_x_val = 0, global_x_int;
pthread_mutex_t global_result_lock;

struct arguments
{
  float a, b, result, x_val = 0, x_int;
  int intensity, func, n, start, end;
};

//This function has the code for thread level mutual exclusion. Every thread has their own instance of result variable and it is aggregated later.
void *integrate_thread_level(void *argument)
{
  struct arguments *arg = (struct arguments *)argument;

  for (int i = arg->start; i < arg->end; i++)
  {
    arg->x_int = (arg->a + ((float)i + 0.5) * ((arg->b - arg->a) / (float)arg->n));
    switch (arg->func)
    {
      case 1:
      {
        arg->x_val = arg->x_val + f1(arg->x_int, arg->intensity);
        break;
      }
      case 2:
      {
        arg->x_val = arg->x_val + f2(arg->x_int, arg->intensity);
        break;
      }
      case 3:
      {
        arg->x_val = arg->x_val + f3(arg->x_int, arg->intensity);
        break;
      }
      case 4:
      {
        arg->x_val = arg->x_val + f4(arg->x_int, arg->intensity);
        break;
      }
      default:
        std::cout << "\nWrong function id" << std::endl;
    }
    arg->result = arg->x_val * ((arg->b - arg->a) / (float)arg->n);
  }
  pthread_exit(NULL);
}

//This function has the code for iteration level mutual exclusion. Every thread manipulates the global variable in the inner most loop through access in the critical section.
void *integrate_iteration_level(void *argument)
{
  struct arguments *arg = (struct arguments *)argument;
  for (int i = arg->start; i < arg->end; i++)
  {
    pthread_mutex_lock(&global_result_lock);
    global_x_int = arg->a + ((float)i + 0.5) * ((arg->b - arg->a) / (float)arg->n);
    switch (arg->func)
    {
      case 1:
      {
        global_x_val = global_x_val + f1(global_x_int, arg->intensity);
        break;
      }
      case 2:
      {
        global_x_val = global_x_val + f2(global_x_int, arg->intensity);
        break;
      }
      case 3:
      {
        global_x_val = global_x_val + f3(global_x_int, arg->intensity);
        break;
      }
      case 4:
      {
        global_x_val = global_x_val + f4(global_x_int, arg->intensity);
        break;
      }
      default:
        std::cout << "\nWrong function id" << std::endl;
    }
    global_result = global_x_val * ((arg->b - arg->a) / (float)arg->n);
    pthread_mutex_unlock(&global_result_lock);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

  float final_result = 0, x_val = 0, x_int, a, b;
  double cpu_time;
  int func, intensity, nbthreads, n;
  char *sync;
  func = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  sync = argv[7];

  pthread_t *threads;

  struct arguments *arg;

  threads = (pthread_t *)malloc(nbthreads * sizeof(pthread_t));
  arg = (struct arguments *)malloc(nbthreads * sizeof(struct arguments));

  pthread_mutex_init(&global_result_lock, NULL);

  auto clock_start = std::chrono::system_clock::now();

  if (strcmp(sync, "thread") == 0)
  {
    for (int j = 0; j < nbthreads; j++)
    {
      arg[j].a = a;
      arg[j].b = b;
      arg[j].start = j * (n / nbthreads);
      arg[j].end = arg[j].start + (n / nbthreads);
      arg[j].intensity = intensity;
      arg[j].func = func;
      arg[j].n = n;
      pthread_create(&threads[j], NULL, integrate_thread_level, (void *)&(arg[j]));
    }
    for (int i = 0; i < nbthreads; i++)
    {
      pthread_join(threads[i], NULL);
    }
    for (int k = 0; k < nbthreads; k++)
    {
      global_result += arg[k].result;
    }
  }
  else
  {
    for (int j = 0; j < nbthreads; j++)
    {
      arg[j].a = a;
      arg[j].b = b;
      arg[j].start = j * (n / nbthreads);
      arg[j].end = arg[j].start + (n / nbthreads);
      arg[j].intensity = intensity;
      arg[j].func = func;
      arg[j].n = n;
      pthread_create(&threads[j], NULL, integrate_iteration_level, (void *)&(arg[j]));
    }

    for (int j = 0; j < nbthreads; j++)
    {
      pthread_join(threads[j], NULL);
    }
  }

  auto clock_end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = clock_end - clock_start;
  std::cout << global_result;
  std::cerr << diff.count();

  return 0;
}
