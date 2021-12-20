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
  float a, b, result, sum = 0, x, t1;
  int intensity, func, n, start, end;
};

float global_sum = 0, global_x_sum = 0, global_x;
pthread_mutex_t global_sum_lock;

void *thread_sync(void *argument)
{
  struct static_data *thread_data = (struct static_data *)argument;
  for (int i = thread_data->start; i < thread_data->end; i++)
  {
    thread_data->x = (thread_data->a + ((float)i + 0.5) * thread_data->t1);
    switch (thread_data->func)
    {
      case 1:
        thread_data->sum += f1(thread_data->x, thread_data->intensity);
        break;
      case 2:
        thread_data->sum += f2(thread_data->x, thread_data->intensity);
        break;
      case 3:
        thread_data->sum += f3(thread_data->x, thread_data->intensity);
        break;
      case 4:
        thread_data->sum += f4(thread_data->x, thread_data->intensity);
        break;
      default:
        std::cout << "\nWrong function id" << std::endl;
    }
    thread_data->result = thread_data->sum * thread_data->t1;
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
        global_x_sum += f1(global_x, thread_data->intensity);
        break;
      case 2:
        global_x_sum += f2(global_x, thread_data->intensity);
        break;
      case 3:
        global_x_sum += f3(global_x, thread_data->intensity);
        break;
      case 4:
        global_x_sum += f4(global_x, thread_data->intensity);
        break;
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

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }
  
  float final_result = 0, a, b, local_t1;
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

  local_t1 = (b-a)/(float)n;
  std::chrono::time_point<std::chrono::system_clock> clock_start = std::chrono::system_clock::now();

  if (strcmp(sync, "thread") == 0)
  {
    for (int i = 0; i < nbthreads; i++)
    {
      thread_data[i].a = a;
      thread_data[i].b = b;
      thread_data[i].start = i * (n / nbthreads);
      thread_data[i].end = thread_data[i].start + (n / nbthreads);
      thread_data[i].intensity = intensity;
      thread_data[i].func = func;
      thread_data[i].n = n;
      thread_data[i].t1 = local_t1;
      pthread_create(&threads[i], NULL, thread_sync, (void *)&(thread_data[i]));
    }
    for (int j = 0; j < nbthreads; j++)
    {
      pthread_join(threads[j], NULL);
    }
    for (int m = 0; m < nbthreads; m++)
    {
      global_sum += thread_data[m].result;
    }
  }
  else if(strcmp(sync, "iteration") == 0 )
  {
    for (int i = 0; i < nbthreads; i++)
    {
      thread_data[i].a = a;
      thread_data[i].b = b;
      thread_data[i].start = i * (n / nbthreads);
      thread_data[i].end = thread_data[i].start + (n / nbthreads);
      thread_data[i].intensity = intensity;
      thread_data[i].func = func;
      thread_data[i].n = n;
      thread_data[i].t1 = local_t1;
      pthread_create(&threads[i], NULL, iteration_sync, (void *)&(thread_data[i]));
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
