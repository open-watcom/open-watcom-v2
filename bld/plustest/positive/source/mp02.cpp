#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

struct S {
    int i,j;
};

int S::* foo( int x )
{
    return ( x & 1 ) ? &S::j : &S::i;
}

S x = { -10, 10 };

int main()
{
    if( x.*(foo(2)) != -10 ) { fail(__LINE__); }
    if( x.*(foo(3)) != 10 ) { fail(__LINE__); }
    _PASS;
}
