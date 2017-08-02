#include "fail.h"

int da1( int = 3 );
double da2( double = 3.875 );

struct S : _CD {
    int s;
    S( int s ) :s(s) {
    }
    ~S() {
	if( s < 0 ) fail(__LINE__);
	s = -1;
    }
};

#if __WATCOM_REVISION__ >= 8
int da3( S const & = 3, S const & = 4 );
int da4( S const & = da3( 5, 6 ) );
#else
int da3( S const & = S(3), S const & = S(4) );
int da4( S const & = S( da3( 5, 6 ) ) );
#endif

struct B : _CD {
    virtual int f( int x ) {
	return x;
    }
};

struct D : virtual B {
    virtual int f( int x ) {
	return x + 1;
    }
};

D __v;
B *p = &__v;

int da5( int = p->f(2) );

int da1( int x ) {
    return x + 1;
}

double da2( double x ) {
    return x + 1;
}

int da3( S const &x, S const &y ) {
    return x.s + y.s;
}

int da4( S const &x ) {
    return x.s + 1;
}

int da5( int x ) {
    return x + 1;
}
