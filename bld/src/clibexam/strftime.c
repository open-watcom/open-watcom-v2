#include <stdio.h>
#include <time.h>
 
int main( void ) {
    time_t time_of_day;
    char buffer[ 80 ];
 
    time_of_day = time( NULL );
    strftime( buffer, 80, "Today is %A %B %d, %Y",
               localtime( &time_of_day ) );
    printf( "%s\n", buffer );
    strftime( buffer, 80, "in ISO format %F %R, ISO weeknumber %V",
               localtime( &time_of_day ) );
    printf( "%s\n", buffer );
    return ( 0 );
}
