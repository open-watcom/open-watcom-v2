#include <conio.h>
#include <stdarg.h>

void cfind( char *format, ... )
  {
    va_list arglist;

    va_start( arglist, format );
    vcscanf( format, arglist );
    va_end( arglist );
  }

void main()
  {
    int day, year;
    char weekday[10], month[12];

    cfind( "%s %s %d %d",
            weekday, month, &day, &year );
    cprintf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }
