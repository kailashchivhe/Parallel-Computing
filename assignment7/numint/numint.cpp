#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

double calc_numerical_integration(int functionid, float a, float b, int n, int intensity)
{
    float sum=0;
    float w = (b-a)/(float)n;
    float x;

    if ( functionid == 1 ){
      #pragma omp parallel for 
          for (int i=0;i<n;i++) {
            x = a + (((float)i + 0.5) * w);
            sum= sum + f1(x, intensity);
          }
      return (sum*w);
    }else if ( functionid == 2 ){
        #pragma omp parallel for 
          for (int i=0;i<n;i++) {
            x = a + (((float)i + 0.5) * w);
            sum= sum + f2(x, intensity);
          }
      return (sum*w);
    }else if ( functionid == 3) {
        #pragma omp parallel for 
          for (int i=0;i<n;i++) {
            x = a + (((float)i + 0.5) * w);
            sum= sum + f3(x, intensity);
          }
        return (sum*w);
    }else if ( functionid == 4 ) {
        #pragma omp parallel for 
          for (int i=0;i<n;i++) {
            x = a + (((float)i + 0.5) * w);
            sum= sum + f4(x, intensity);
          }
        return (sum*w);
    }
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
    
  if (kind.compare("static")==0)
  {
    #pragma omp parallel
    #pragma omp omp_set_schedule(static)
      result=calc_numerical_integration(function_id, a, b, n, intensity);
  }
  else if (kind.compare("dynamic")==0){
    #pragma omp parallel
    #pragma omp omp_set_schedule(dynamic)
      result=calc_numerical_integration(function_id, a, b, n, intensity);
  }
  else if (kind.compare("guided")==0){
    #pragma omp parallel
    #pragma omp omp_set_schedule(guided)
      result=calc_numerical_integration(function_id, a, b, n, intensity);
  }
 
  gettimeofday(&end, NULL);
  
  cout<<result; 
  		
  double time = ((double)end.tv_sec-(double)start.tv_sec+((double)end.tv_usec-(double)start.tv_usec)/(double)1000000); // in seconds
  
  std::cerr<<time;

  return 0;
}
