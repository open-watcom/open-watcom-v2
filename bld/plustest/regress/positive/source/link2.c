#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

struct S {
    int a;
    S(int x ) : a(x) {}
};
extern "C" void weird_C_function( S &r, S *p )
{
    if( &r != p ) {
	fail(__LINE__);
    } else {
	r.a = 1;
    }
}
int main()
{
    S x(-1);

    weird_C_function( x, &x );
    if( x.a != 1 ) {
	fail(__LINE__);
    }
    _PASS;
}
