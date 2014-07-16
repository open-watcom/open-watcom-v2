#include "fail.h"

#if __WATCOMC__ > 1060

struct S1 {
    int v;
    virtual int get() {
	return v;
    }
    S1( int v ) : v(v) {
    }
};

struct S2 : S1 {
    virtual int get( int x )
    {
	v ^= x;
	x ^= v;
	v ^= x;
	return x;
    }
    S2( int v ) : S1(v) {
    }
};

struct S3 : S2 {
    int get() {
	return v + 1;
    }
    virtual int get( int x )
    {
	v ^= x;
	x ^= v;
	v ^= x;
	return x + 1;
    }
    S3( int v ) : S2(v) {
    }
};

int main()
{
    S2 *p = new S3(3);
    S1 *q = p;

    if( q->get() != 4 ) fail(__LINE__);
    if( p->get(2) != 4 ) fail(__LINE__);
    if( q->get() != 3 ) fail(__LINE__);
    if( p->get(1) != 3 ) fail(__LINE__);
    if( q->get() != 2 ) fail(__LINE__);
    if( p->get(1) != 2 ) fail(__LINE__);
    _PASS;
}

#else

int main()
{
    _PASS;
}

#endif
