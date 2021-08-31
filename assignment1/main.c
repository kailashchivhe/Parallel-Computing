#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

double calc_numerical_integration(int function_id, int a, int b, int n, int intensity)
{
    float x;
    float t1 = (b - a) / n;
    float sum = 0;
    for( int i=0 ; i<n ; i++ )
    {
        x = a + ((i + 0.5) * t1);
        switch(function_id)
        {
            case 1: sum += f1(x,intensity);
                break;
            case 2: sum += f2(x,intensity);
                break;
            case 3: sum += f3(x,intensity);
                break;
            case 4: sum += f4(x,intensity);
                break;
            default: return -1;
        }
    }
    return (t1*sum);
}

int main (int argc, char* argv[]) {
    
    int function_id, a, b, n, intensity;

    if (argc < 6) {
        fprintf(stderr, "usage: %s <functionid> <a> <b> <n> <intensity>", argv[0]);
        return -1;
    }
    
    function_id = atoi(argv[0]);
    a = atoi(argv[1]);
    b = atoi(argv[2]);
    n = atoi(argv[3]);
    intensity = atoi(argv[4]);

    clock_t t; // t represents clock ticks which is of type 'clock_t'
    t = clock(); // start clock
    double r = calc_numerical_integration(function_id, a, b, n, intensity);
    t = clock()-t; // end clock=
    float time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds; CLOCKS_PER_SEC is the number of clock ticks per second
    printf("%lf\n", r);
    fprintf(stderr, "%f\n", time_taken);
    
    return 0;
}

