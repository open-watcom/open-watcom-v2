#include <stdio.h>
#include <jtime.h>

void main()
  {
    struct tm  time_of_day;
    time_t     ltime;

    time( &ltime );
    _localtime( &ltime, &time_of_day );
    printf( "Date and time is: %s\n",
            jasctime( &time_of_day ) );
  }
