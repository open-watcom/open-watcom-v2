#include <stdio.h>
#include <stdarg.h>

void sfind( char *string, char *format, ... )
  {
    va_list arglist;

    va_start( arglist, format );
    vsscanf( string, format, arglist );
    va_end( arglist );
  }

void main()
  {
    int day, year;
    char weekday[10], month[12];

    sfind( "Saturday April 18 1987",
            "%s %s %d %d",
            weekday, month, &day, &year );
    printf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }
