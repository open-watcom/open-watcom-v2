#include "fail.h"
#include <iostream.h>
#include <stdlib.h>

enum E { A, B, C };

int inc( signed char x )
{
    return x + 2;
}

int inc( unsigned char x )
{
    return x + 2;
}

int inc( char x )
{
    return x + 1;
}

int inc( E x )
{
    return x + 1;
}

struct S {
    E a : 2;
    char b : 5;
    unsigned char c : 3;
};

int main()
{
    int *p(0);
    S x;
    E d = B;
    E *q(&d);
    char c = '@';
    char *r = &c;

    if( p != NULL ) fail(__LINE__);
    x.a = A;
    x.b = 0;
    x.c = 0;
    if( inc( x.a ) != B ) fail(__LINE__);
    if( inc( x.b ) != 1 ) fail(__LINE__);
    if( inc( x.c ) != 2 ) fail(__LINE__);
    if( inc( *q ) != C ) fail(__LINE__);
    if( inc( *r ) != ('@'+1) ) fail(__LINE__);
    _PASS;
}
