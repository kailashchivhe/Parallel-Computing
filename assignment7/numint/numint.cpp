#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

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

float getFunctionResult(int functionId, float x, int intensity )
{
  switch(functionid){
      case 1:
      {
        return f1(x, intensity);
      }
      case 2:
      {
        return f2(x, intensity);
      }
      case 3:
      {
        return f3(x, intensity);
      }
      case 4:
      {
        return f4(x, intensity);
      }
  }
  return 0; 
}

double calc_numerical_integration(int functionid, float a, float b, int n, int intensity, int nbthreads )
{
    float sum=0;
    float w = (b-a)/(float)n;
    float x;
    #pragma omp parallel for reduction(+: sum) schedule(runtime) num_threads(nbthreads)
    for (int i=0;i<n;i++) 
    {
      x = a + (((float)i + 0.5) * w);
      sum += getFunctionResult( functionid, x, intensity);
    }
    // switch(functionid){
    //   case 1:
    //   {
    //     #pragma omp parallel for reduction(+: sum) schedule(runtime) num_threads(nbthreads)
    //     for (int i=0;i<n;i++) 
    //     {
    //         x = a + (((float)i + 0.5) * w);
    //         sum += f1(x, intensity);
    //     }
    //     break;
    //   }
    //   case 2:
    //   {
    //     #pragma omp parallel for reduction(+: sum) schedule(runtime) num_threads(nbthreads)
    //     for (int i=0;i<n;i++) 
    //     {
    //         x = a + (((float)i + 0.5) * w);
    //         sum += f2(x, intensity);
    //     }
    //     break;
    //   }
    //   case 3:
    //   {
    //     #pragma omp parallel for reduction(+: sum) schedule(runtime) num_threads(nbthreads)
    //     for (int i=0;i<n;i++) 
    //     {
    //         x = a + (((float)i + 0.5) * w);
    //         sum += f3(x, intensity);
    //     }
    //     break;
    //   }
    //   case 4:
    //   {
    //     #pragma omp parallel for reduction(+: sum) schedule(runtime)
    //     for (int i=0;i<n;i++) 
    //     {
    //         x = a + (((float)i + 0.5) * w);
    //         sum += f4(x, intensity);
    //     }
    //     break;
    //   }
    // }
    return (sum*w);
}

int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
  #pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  //insert code here

  float a, b,result;
   
  int function_id, intensity, nbthreads,n;
  function_id = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  string kind = argv[7];  

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  if (kind.compare("static")==0)
  {
    omp_set_schedule(omp_sched_static,-1);
  }
  else if (kind.compare("dynamic")==0){
    omp_set_schedule(omp_sched_dynamic,-1);
  }
  else if (kind.compare("guided")==0){
    omp_set_schedule(omp_sched_guided,-1);
  }

  result = calc_numerical_integration(function_id, a, b, n, intensity, nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;

  cout<<result; 

  std::cerr<<elapased_seconds.count()<<std::endl;

  return 0;
}
