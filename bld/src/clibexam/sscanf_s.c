#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main( void )
{
    int     day, year;
    char    weekday[10], month[10];

    sscanf_s( "Friday August 0013 2004",
            "%s %s %d  %d",
             weekday, sizeof( weekday ),
             month, sizeof( month ),
             &day, &year );
    printf_s( "%s %s %d %d\n",
             weekday, month, day, year );
}
