#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct S {
    int a;
    char b[];
};

S *foo()
{
    if( sizeof( S ) != sizeof( int ) ) fail(__LINE__);
    S *p = (S*) malloc( sizeof( int ) + 4 );
    p->a = -1;
    p->b[0] = 'a';
    p->b[1] = 'b';
    p->b[2] = 'c';
    p->b[3] = '\0';
    return p;
}

int main()
{
    S *x = foo();
    if( x->a != -1 && strcmp( x->b, "abc" ) != 0 ) fail(__LINE__);
    _PASS;
}
