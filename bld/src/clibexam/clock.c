#include <stdio.h>
#include <math.h>
#include <time.h>

void compute( void )
  {
    int i, j;
    double x;

    x = 0.0;
    for( i = 1; i <= 100; i++ )
      for( j = 1; j <= 100; j++ )
        x += sqrt( (double) i * j );
    printf( "%16.7f\n", x );
  }

void main()
  {
    clock_t start_time, end_time;

    start_time = clock();
    compute();
    end_time = clock();
    printf( "Execution time was %lu seconds\n",
          (end_time - start_time) / CLOCKS_PER_SEC );
  }
