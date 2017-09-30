#include "dump.h"
char c = 'c';
char &rc = c;
const signed char &rsc = c;	// ok, create temporary
const unsigned char &ruc = c;	// ok, create temporary
int main( void )
{
    printf( "%c\n", c );
    printf( "%c\n", rc );
    printf( "%c\n", rsc );
    printf( "%c\n", ruc );
    return 0;
}
