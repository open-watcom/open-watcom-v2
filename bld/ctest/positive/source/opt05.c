#include "fail.h"
#include <stdio.h>
#include <string.h>

void clr( char **b, unsigned *s )
{
    int i;

    for( i = 0; i < *s; ++i ) {
	(*b)[i] = 0;
    }
}

int main()
{
    char buff[10];
    char cbuff[10];
    unsigned s = sizeof( buff );
    char *p[2];

    p[0] = buff;
    p[1] = NULL;
    clr( p, &s );
    memset( cbuff, 0, sizeof( cbuff ) );
    if( memcmp( buff, cbuff, sizeof( buff ) ) != 0 ) fail(__LINE__);
    _PASS;
}
