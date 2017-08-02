#include <string.h>
#include "fail.h"

const float r = 23;

void foo( char *s )
{
    if( r != 23 ) fail(__LINE__);
}

int main()
{
    char a[512];

    memset( a, 0, sizeof( a ) );
    foo( a );
    _PASS;
}
