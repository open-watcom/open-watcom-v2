#include <stdio.h>
#include <strings.h>

int main( void )
{
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX", 10 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  6 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  3 ) );
    printf( "%d\n", strncasecmp( "abcdef", "ABCXXX",  0 ) );
    return( 0 );
}
