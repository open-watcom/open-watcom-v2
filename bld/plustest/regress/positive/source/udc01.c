#include "fail.h"

#ifdef __WATCOM_BOOL__

struct B { int b; };
struct D : virtual B {
    int d;
};

D dd;

struct T {
    operator D*() {
	return p;
    };
    D *p;
    T( D *p ) : p(p) {
    }
};

void try_B( B *p )
{
    if( p != &dd ) fail(__LINE__);
    if( p->b != 'b' ) fail(__LINE__);
}

struct S {
    S( int s ) : s(s) {
    }
    operator int() {
	return s;
    }
    int s;
};

void test_bool( bool b, int v )
{
    if( v ) {
	if( b == false ) fail(__LINE__);
	if( b != true ) fail(__LINE__);
	if( ++b != true ) fail(__LINE__);
	if( ++b != true ) fail(__LINE__);
    } else {
	if( b != false ) fail(__LINE__);
	if( b == true ) fail(__LINE__);
	if( ++b != true ) fail(__LINE__);
	if( ++b != true ) fail(__LINE__);
    }
}

void try_bool( int v ) {
    S ss(v);

    test_bool( ss, v );
}


int main() {
    dd.d = 'd';
    dd.b = 'b';
    T tt( &dd );
    try_B( tt );
    try_bool(0);
    try_bool(1);
    try_bool(-1);
    _PASS;
}

#else

ALWAYS_PASS

#endif
