#include <stdio.h>
#include <time.h>

void main()
  {
    time_t time_of_day;
    auto char buf[26];

    time_of_day = time( NULL );
    printf( "It is now: %s", _ctime( &time_of_day, buf ) );
  }
