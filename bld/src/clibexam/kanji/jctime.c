#include <stdio.h>
#include <jtime.h>

void main()
  {
    time_t time_of_day;

    time_of_day = time( NULL );
    printf( "It is now: %s", jctime( &time_of_day ) );
  }
