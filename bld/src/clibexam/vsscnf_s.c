#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdarg.h>

void sfind( char *string, char *format, ... )
{
    va_list arglist;

    va_start( arglist, format );
    vsscanf_s( string, format, arglist );
    va_end( arglist );
}

void main( void )
{
    int     day, year;
    char    weekday[10], month[10];

    sfind( "Friday August 0013 2004",
            "%s %s %d %d",
             weekday, sizeof( weekday ),
             month, sizeof( month ),
             &day, &year );
    printf_s( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
}
