#include <stdio.h>
#include <stdarg.h>

void find( char *format, ... )
  {
    va_list arglist;

    va_start( arglist, format );
    vscanf( format, arglist );
    va_end( arglist );
  }

void main()
  {
    int day, year;
    char weekday[10], month[12];

    find( "%s %s %d %d",
            weekday, month, &day, &year );
    printf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }
