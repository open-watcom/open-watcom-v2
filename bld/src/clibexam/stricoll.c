#include <stdio.h>
#include <string.h>
#include <mbstring.h>

char buffer[80] = "world";

void main()
{
    int     test;

    _setmbcp( 932 );
    test = _stricoll( buffer, "Hello" );
    if( test < 0 ) {
        printf( "Less than\n" );
    } else if( test == 0 ) {
        printf( "Equal\n" );
    } else {
        printf( "Greater than\n" );
    }

}
