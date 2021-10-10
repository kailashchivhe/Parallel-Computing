#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <chrono>
#include <cmath>

using namespace std;

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

using seconds = chrono::seconds;
using check_time = std::chrono::high_resolution_clock;

struct dynamic_data
{
	string sync;
	float a, b, t1;
	int begin, end, functionid, intensity, n;
};

pthread_mutex_t mut_sum, mutex_comp;
float total_sum = 0.0, t1 = 0.0;
int computations_left = 0, n, global_begin = 0, global_end = 0, granularity = 0;

bool work_done()
{
	pthread_mutex_lock(&mutex_comp);
	if (computations_left == 0)
	{
		pthread_mutex_unlock(&mutex_comp);
		return true;
	}
	else
	{
		if (computations_left < granularity)
		{
			computations_left = 0;
		}
		else
		{
			computations_left = computations_left - granularity;
		}
		pthread_mutex_unlock(&mutex_comp);
		return false;
	}
}

void get_begin_and_end(int *begin, int *end)
{
	pthread_mutex_lock(&mutex_comp);
	*begin = global_begin;
	*end = global_end;

	if (granularity <= (n - global_end))
	{
		global_begin = *end;
		global_end = global_begin + granularity;
	}
	pthread_mutex_unlock(&mutex_comp);
}

void *iteration_sync(void *p)
{
	int begin, end;
	while (!work_done())
	{
		dynamic_data *thread_data = (dynamic_data *)p;

		get_begin_and_end(&begin, &end);

		for (int i = begin; i < end; i++)
		{
			float x = (thread_data->a + (i + 0.5) * thread_data->t1);
			switch (thread_data->functionid)
			{
				case 1:
					pthread_mutex_lock(&mut_sum);
					total_sum += (f1(x, thread_data->intensity) * thread_data->t1);
					pthread_mutex_unlock(&mut_sum);
					break;
				case 2:
					pthread_mutex_lock(&mut_sum);
					total_sum += (f2(x, thread_data->intensity) * thread_data->t1);
					pthread_mutex_unlock(&mut_sum);
					break;
				case 3:
					pthread_mutex_lock(&mut_sum);
					total_sum += (f3(x, thread_data->intensity) * thread_data->t1);
					pthread_mutex_unlock(&mut_sum);
					break;
				case 4:
					pthread_mutex_lock(&mut_sum);
					total_sum += (f4(x, thread_data->intensity) * thread_data->t1);
					pthread_mutex_unlock(&mut_sum);
					break;
				default:
        			std::cout << "\nWrong function id" << std::endl;
			}
		}
	}
	pthread_exit(NULL);
}

void *chunk_sync(void *p)
{
	while (!work_done())
	{
		dynamic_data *thread_data = (dynamic_data *)p;
		int begin, end;
		float sum = 0.0;
		get_begin_and_end(&begin, &end);
		for (int i = begin; i < end; i++)
		{
			float x = (thread_data->a + (i + 0.5) * thread_data->t1);
			switch (thread_data->functionid)
			{
			case 1:
				sum += (f1(x, thread_data->intensity));
				break;
			case 2:
				sum += (f2(x, thread_data->intensity));
				break;
			case 3:
				sum += (f3(x, thread_data->intensity));
				break;
			case 4:
				sum += (f4(x, thread_data->intensity));
				break;
			default:
        		std::cout << "\nWrong function id" << std::endl;
			}
		}
		pthread_mutex_lock(&mut_sum);
		total_sum += sum;
		pthread_mutex_unlock(&mut_sum);
	}
	pthread_exit(NULL);
}

void *thread_sync(void *p)
{
	float sum = 0.0;
	int begin, end;
	dynamic_data *thread_data = (dynamic_data *)p;
	while (!work_done())
	{
		get_begin_and_end(&begin, &end);
		for (int i = begin; i < end; i++)
		{
			float x = (thread_data->a + (i + 0.5) * thread_data->t1);

			switch (thread_data->functionid)
			{
			case 1:
				sum += (f1(x, thread_data->intensity));
				break;
			case 2:
				sum += (f2(x, thread_data->intensity));
				break;
			case 3:
				sum += (f3(x, thread_data->intensity));
				break;
			case 4:
				sum += (f4(x, thread_data->intensity));
				break;
			default:
        		std::cout << "\nWrong function id" << std::endl;
			}
		}
	}
	pthread_mutex_lock(&mut_sum);
	total_sum = total_sum + sum;
	pthread_mutex_unlock(&mut_sum);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();

	if (argc < 9)
	{
		std::cerr << "usage: " << argv[0] << " <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>" << std::endl;
		return -1;
	}
	int functionid = atoi(argv[1]);
	float a = atof(argv[2]);
	float b = atof(argv[3]);
	n = atoi(argv[4]);
	int intensity = atoi(argv[5]);
	float t1 = ((b - a) / n);
	int nbthreads = atoi(argv[6]);
	string sync = argv[7];
	granularity = atoi(argv[8]);
	global_end = granularity;
	struct dynamic_data thread_data[nbthreads];
	pthread_t thread[nbthreads];
	computations_left = n;

	pthread_mutex_init(&mut_sum, NULL);
	pthread_mutex_init(&mutex_comp, NULL);

	if (0 == sync.compare("thread"))
	{
		for (int i = 0; i < nbthreads; i++)
		{
			thread_data[i].a = a;
			thread_data[i].b = b;
			thread_data[i].functionid = functionid;
			thread_data[i].intensity = intensity;
			thread_data[i].t1 = t1;
			thread_data[i].sync = sync;
			thread_data[i].n = n;
			pthread_create(&thread[i], NULL, thread_sync, (void *)&thread_data[i]);
		}
	}
	else if (0 == sync.compare("chunk"))
	{
		for (int i = 0; i < nbthreads; i++)
		{
			thread_data[i].a = a;
			thread_data[i].b = b;
			thread_data[i].functionid = functionid;
			thread_data[i].intensity = intensity;
			thread_data[i].t1 = t1;
			thread_data[i].sync = sync;
			thread_data[i].n = n;
			pthread_create(&thread[i], NULL, chunk_sync, (void *)&thread_data[i]);
		}
	}
	else if (0 == sync.compare("iteration"))
	{
		for (int i = 0; i < nbthreads; i++)
		{
			thread_data[i].a = a;
			thread_data[i].b = b;
			thread_data[i].functionid = functionid;
			thread_data[i].intensity = intensity;
			thread_data[i].t1 = t1;
			thread_data[i].sync = sync;
			thread_data[i].n = n;
			pthread_create(&thread[i], NULL, iteration_sync, (void *)&thread_data[i]);
		}
	}

	for (int i = 0; i < nbthreads; i++)
	{
		pthread_join(thread[i], NULL);
	}

	pthread_mutex_destroy(&mut_sum);
	pthread_mutex_destroy(&mutex_comp);

	cout << total_sum * t1;

	std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end_time - start_time;
	std::cerr << elapsed_seconds.count() << std::endl;

	return 0;
}