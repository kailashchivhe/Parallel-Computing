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
struct static_data
{
  float a, b, result, x_val = 0, x_int, t1;
  int intensity, func, n, start, end;
};

float global_sum = 0, global_x_sum = 0, global_x;
pthread_mutex_t global_sum_lock;

void *thread_sync(void *argument)
{
  struct static_data *thread_data = (struct static_data *)argument;

  for (int i = thread_data->start; i < thread_data->end; i++)
  {
    thread_data->x_int = (thread_data->a + ((float)i + 0.5) * thread_data->t1);
    switch (thread_data->func)
    {
      case 1:
      {
        thread_data->x_val += f1(thread_data->x_int, thread_data->intensity);
        break;
      }
      case 2:
      {
        thread_data->x_val += f2(thread_data->x_int, thread_data->intensity);
        break;
      }
      case 3:
      {
        thread_data->x_val += f3(thread_data->x_int, thread_data->intensity);
        break;
      }
      case 4:
      {
        thread_data->x_val += f4(thread_data->x_int, thread_data->intensity);
        break;
      }
      default:
        std::cout << "\nWrong function id" << std::endl;
    }
    thread_data->result = thread_data->x_val * thread_data->t1;
  }
  pthread_exit(NULL);
}

void *iteration_sync(void *argument)
{
  struct static_data *thread_data = (struct static_data *)argument;
  for (int i = thread_data->start; i < thread_data->end; i++)
  {
    pthread_mutex_lock(&global_sum_lock);
    global_x = thread_data->a + ((float)i + 0.5) * thread_data->t1;
    switch (thread_data->func)
    {
      case 1:
      {
        global_x_sum += f1(global_x, thread_data->intensity);
        break;
      }
      case 2:
      {
        global_x_sum += f2(global_x, thread_data->intensity);
        break;
      }
      case 3:
      {
        global_x_sum += f3(global_x, thread_data->intensity);
        break;
      }
      case 4:
      {
        global_x_sum += f4(global_x, thread_data->intensity);
        break;
      }
      default:
        std::cout << "\nWrong function id" << std::endl;
    }
    global_sum = global_x_sum * thread_data->t1;
    pthread_mutex_unlock(&global_sum_lock);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

  float final_result = 0, a, b;
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

  struct static_data *thread_data;

  threads = (pthread_t *)malloc(nbthreads * sizeof(pthread_t));
  thread_data = (struct static_data *)malloc(nbthreads * sizeof(struct static_data));

  pthread_mutex_init(&global_sum_lock, NULL);

  std::chrono::time_point<std::chrono::system_clock> clock_start = std::chrono::system_clock::now();

  if (strcmp(sync, "thread") == 0)
  {
    for (int j = 0; j < nbthreads; j++)
    {
      thread_data[j].a = a;
      thread_data[j].b = b;
      thread_data[j].start = j * (n / nbthreads);
      thread_data[j].end = thread_data[j].start + (n / nbthreads);
      thread_data[j].intensity = intensity;
      thread_data[j].func = func;
      thread_data[j].n = n;
      thread_data[j].t1 = (b-a)/(float)n;
      pthread_create(&threads[j], NULL, thread_sync, (void *)&(thread_data[j]));
    }
    for (int i = 0; i < nbthreads; i++)
    {
      pthread_join(threads[i], NULL);
    }
    for (int k = 0; k < nbthreads; k++)
    {
      global_sum += thread_data[k].result;
    }
  }
  else if(strcmp(sync, "iteration") == 0 )
  {
    for (int j = 0; j < nbthreads; j++)
    {
      thread_data[j].a = a;
      thread_data[j].b = b;
      thread_data[j].start = j * (n / nbthreads);
      thread_data[j].end = thread_data[j].start + (n / nbthreads);
      thread_data[j].intensity = intensity;
      thread_data[j].func = func;
      thread_data[j].n = n;
      thread_data[j].t1 = (b-a)/(float)n;
      pthread_create(&threads[j], NULL, iteration_sync, (void *)&(thread_data[j]));
    }

    for (int j = 0; j < nbthreads; j++)
    {
      pthread_join(threads[j], NULL);
    }
  }

  std::chrono::time_point<std::chrono::system_clock> clock_end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = clock_end - clock_start;
  std::cout << global_sum;
  std::cerr << diff.count();

  return 0;
}
