#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main( void )
{
    int     day, year;
    char    weekday[10], month[10];
    FILE    *in_data;

    in_data = fopen( "file", "r" );
    if( in_data != NULL ) {
        fscanf_s( in_data, "%s %s %d %d",
                weekday, sizeof( weekday ),
                month, sizeof( month ),
                &day, &year );
        printf_s( "Weekday=%s Month=%s Day=%d Year=%d\n",
                weekday, month, day, year );
        fclose( in_data );
    }
}
