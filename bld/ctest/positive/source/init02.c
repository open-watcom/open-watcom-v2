#include "fail.h"
#include <stdlib.h>

enum E { A, B, C };

int incs( signed char x )
{
    return x + 2;
}

int incu( unsigned char x )
{
    return x + 2;
}

int incc( char x )
{
    return x + 1;
}

int ince( enum E x )
{
    return x + 1;
}

struct S {
    enum E a : 2;
    char b : 5;
    unsigned char c : 3;
};

int main()
{
    int *p = 0;
    struct S x;
    enum E d = B;
    enum E *q = &d;
    char c = '@';
    char *r = &c;

    if( p != NULL ) fail(__LINE__);
    x.a = A;
    x.b = 0;
    x.c = 0;
    if( ince( x.a ) != B ) fail(__LINE__);
    if( incc( x.b ) != 1 ) fail(__LINE__);
    if( incu( x.c ) != 2 ) fail(__LINE__);
    if( ince( *q ) != C ) fail(__LINE__);
    if( incc( *r ) != ('@'+1) ) fail(__LINE__);
    _PASS;
}
