
#include <stdio.h>
#include <stdarg.h>

void errmsg( char *format, ... )
  {
    va_list arglist;

    printf( "Error: " );
    va_start( arglist, format );
    vprintf( format, arglist );
    va_end( arglist );
  }

void main()
  {
    errmsg( "%s %d %s", "Failed", 100, "times" );
  }

