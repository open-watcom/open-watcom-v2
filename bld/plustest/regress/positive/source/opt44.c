#include "fail.h"
#include <string.h>

struct S {
    unsigned short a,b;
    unsigned char c,d,e,f;
};

void foo( S *p ) {
    p->a = 1;
    p->b = 2;
    p->c = 3;
    p->d = 4;
    p->e = 5;
    p->f = 6;
}

int main()
{
    S x;

    memset( &x, -1, sizeof(x) );
    foo( &x );
    if( x.a != 1 ) _fail;
    if( x.b != 2 ) _fail;
    if( x.c != 3 ) _fail;
    if( x.d != 4 ) _fail;
    if( x.e != 5 ) _fail;
    if( x.f != 6 ) _fail;
    _PASS;
}
