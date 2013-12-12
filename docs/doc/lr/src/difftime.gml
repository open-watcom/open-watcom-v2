.func difftime
#include <time.h>
double difftime( time_t time1, time_t time0 );
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The &func function calculates the difference
between the two calendar times:
.millust begin
    time1 - time0
.millust end
.desc end
.return begin
The &func function returns the difference
between the two times in seconds as a
.id double.
.return end
.see begin
.im seetime difftime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>

void compute( void );

void main()
  {
    time_t start_time, end_time;

    start_time = time( NULL );
    compute();
    end_time = time( NULL );
    printf( "Elapsed time: %f seconds\n",
        difftime( end_time, start_time ) );
  }

void compute( void )
  {
    int i, j;

    for( i = 1; i <= 20; i++ ) {
      for( j = 1; j <= 20; j++ )
        printf( "%3d ", i * j );
      printf( "\n" );
    }
  }
.exmp end
.class ANSI
.system
