#include "fail.h"

#if __WATCOM_REVISION >= 8

template <char &c, int (&i)( int ), int *p, int (*d)( int ) >
    struct S {
	int foo() {
	    return d( i(c) + *p );
	}
    };

int inc( int x ) {
    return x + 1;
}

int dec( int x ) {
    return x - 2;
}

int a;

char x = 'x';
S<x,inc,&a,dec> qx;
char y = 'y';
S<y,inc,&a,&dec> qy;

int main() {
    a = 1;
    if( qx.foo() != ('x'+1+1-2) ) fail(__LINE__);
    x = 'a';
    if( qx.foo() != ('a'+1+1-2) ) fail(__LINE__);
    if( qy.foo() != ('y'+1+1-2) ) fail(__LINE__);
    y = 'A';
    if( qy.foo() != ('A'+1+1-2) ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
