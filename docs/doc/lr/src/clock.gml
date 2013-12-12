.func clock
.synop begin
#include <time.h>
clock_t clock(void);
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The &func function returns the number of clock ticks of processor time
used by program since the program started executing.
This can be converted to seconds by dividing by the value of the macro
.kw CLOCKS_PER_SEC
.ct .li .
.if '&machsys' ne 'QNX' .do begin
.np
Note that under DOS and OS/2, the clock tick counter will reset to 0
for each subsequent 24 hour interval that elapses.
.do end
.desc end
.return begin
The &func function returns the number of clock ticks that have
occurred since the program started executing.
.return end
.see begin
.im seetime clock
.see end
.exmp begin
#include <stdio.h>
#include <math.h>
#include <time.h>
.exmp break
void compute( void )
  {
    int i, j;
    double x;
.exmp break
    x = 0.0;
    for( i = 1; i <= 100; i++ )
      for( j = 1; j <= 100; j++ )
        x += sqrt( (double) i * j );
    printf( "%16.7f\n", x );
  }
.exmp break
void main()
  {
    clock_t start_time, end_time;

    start_time = clock();
    compute();
    end_time = clock();
    printf( "Execution time was %lu seconds\n",
          (end_time - start_time) / CLOCKS_PER_SEC );
  }
.exmp end
.class ANSI
.system
