#include "fail.h"

#ifndef __WATCOM_CPLUSPLUS__
#error WATCOM-specific test case
#endif

/* thunk for V::foo -> S::foo when building an S doesn't need a ctor-disp */
struct V {
    virtual void foo( int x )
    {
	v = x;
    }
    int filler;
    int v;
};

struct S : virtual V {
    int s;
    virtual void foo( int x )
    {
	v = x;
	szero = x;
	int d = (int)&v - (int)&szero;
	// sizeof( int )	-- S::szero
	// sizeof( void * )	-- S::__vfptr
	// sizeof( int )	-- V::filler
	if( d != (sizeof(int)+sizeof(void*)+sizeof(int)) ) {
	    /* no ctor-disp req'd */
	    fail(__LINE__);
	}
    }
    int szero;
};

struct T : S {
    int t;
    virtual void foo( int x )
    {
	v = x;
	tzero = x;
	int d = (int)&v - (int)&tzero;
	// sizeof( int )	-- T::tzero
	// sizeof( int )	-- ctor-disp
	// sizeof( int )	-- V::filler
	if( d != (sizeof(int)+sizeof(int)+sizeof(int)) ) {
	    /* has a ctor-disp */
	    fail(__LINE__);
	}
    }
    inline T()
    {
	foo(11);
    }
    int tzero;
};

int main()
{
    static S x;
    static T y;
    V *p;

    x.foo(17);
    p = &x;
    p->foo(18);
    p = &y;
    p->foo(12);
    _PASS;
}
