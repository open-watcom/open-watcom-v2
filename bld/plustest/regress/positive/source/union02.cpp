#include "fail.h"

struct B {
    int a;
    B(){;}
    B(B const &s) : a(s.a) {;}
};
struct D : B {
    union {
	char b[16];
	unsigned long c[4];
    };
};

void foo( D x, D y ) {
    if( x.a != 'x' ) fail(__LINE__);
    if( x.b[0] != 'x' ) fail(__LINE__);
    if( x.c[1] != 'x' ) fail(__LINE__);
    if( y.a != 'y' ) fail(__LINE__);
    if( y.b[0] != 'y' ) fail(__LINE__);
    if( y.c[1] != 'y' ) fail(__LINE__);
}

int main()
{
    D x, y;

    x.a = 'x';
    x.b[0] = 'x';
    x.c[1] = 'x';
    y.a = 'y';
    y.b[0] = 'y';
    y.c[1] = 'y';
    foo( x, y );
    _PASS;
}
