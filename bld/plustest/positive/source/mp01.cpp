#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

struct S {
    S op( int x )
    {
	return S(x);
    }
    S( int x ) : a (x) {}
    int a;
};

S (S::* mp)( int ) = &S::op;

int main()
{
    S x(2);

    x = (x.*mp)(-1);
    if( x.a != -1 ) fail(__LINE__);
    _PASS;
}
