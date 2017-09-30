#include "fail.h"

#if __WATCOMC__ > 1060

struct C {
    int s;
    static int sig;
    static int ctored;
    C();
    C( C const &r );
    ~C();
};

int C::sig = 1;
int C::ctored;

C::C() : s(++sig) {
    //printf( "C(%p)\n", this );
    ++ctored;
    if( s <= 0 ) fail(__LINE__);
}

C::C( C const &r ) {
    //printf( "C(%p,%p)\n", this, &r );
    /* check of source must occur before any adjustments */
    if( this == &r ) fail(__LINE__);
    if( r.s <= 0 ) fail(__LINE__);
    s = ++sig;
    ++ctored;
    if( s <= 0 ) fail(__LINE__);
}

C::~C() {
    //printf( "~C(%p)\n", this );
    if( s <= 0 ) fail(__LINE__);
    s = -1;
    --ctored;
    if( ctored < 0 ) fail(__LINE__);
}

void cause() {
    throw C();
}

void cause_throw_anew() {
    try {
	cause();
    } catch( C const &r ) {
	if( r.s <= 0 ) fail(__LINE__);
	throw r;
    }
}

void cause_rethrow() {
    try {
	cause();
    } catch( C const &r ) {
	if( r.s <= 0 ) fail(__LINE__);
	throw;
    }
}

int main() {
    try {
	cause_throw_anew();
    } catch( C const &r ) {
	if( C::ctored == 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( C::ctored != 0 ) fail(__LINE__);
    try {
	cause_rethrow();
    } catch( C const &r ) {
	if( C::ctored == 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( C::ctored != 0 ) fail(__LINE__);
    _PASS;
}
#else

ALWAYS_PASS

#endif
