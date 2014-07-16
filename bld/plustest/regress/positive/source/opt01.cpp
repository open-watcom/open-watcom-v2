#include "fail.h"
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <limits.h>

void use( int )
{
}

char *foo( char *p )
{
    use( (*p++) * 0 );
    use( (*p++) % 1 );
    use( 0 % (*p++) );
    use( 0 / (*p++) );
    use( 0 & (*p++) );
    use( -1 | (*p++) );
    use( ((unsigned)(*p++)) << (CHAR_BIT * sizeof(long) * 2 ) );
    use( ((unsigned)(*p++)) >> (CHAR_BIT * sizeof(long) * 2 ) );
    return p;
}

int main()
{
    char buff[16];
    char *p;

    memset( buff, 1, sizeof( buff ) );
    p = foo( buff );
    if( p != &buff[8] ) fail( __LINE__ );
    _PASS;
}
