#include "fail.h"
#include <stdio.h>

void bar( char **p )
{
    if( p[0][0] != 'a' ) fail(__LINE__);
    if( p[1][0] != 'b' ) fail(__LINE__);
}

int main()
{
    char c = 'a';
    char a[40] = { c };
    char b[40] = { 'b' };

    char *p[2] = { a, b };
    bar( p );
    _PASS;
}
