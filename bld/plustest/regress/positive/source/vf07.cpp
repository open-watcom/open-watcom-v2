#include "fail.h"
#include <stdarg.h>
#if 1
#define virtual
#define V1_CHECK	1
#define V2_CHECK	2
#define V3_CHECK	3
#else
// enable if ... and return thunks have to be supported
#define V1_CHECK	-1
#define V2_CHECK	-2
#define V3_CHECK	-3
#endif

// verify that return value thunks don't corrupt ... parms

struct V1 {
    int v;
    V1(int x=1) : v(x) {}
    void c1(int c) { if( v != c ) fail(__LINE__); }
};
struct V2 {
    int v;
    V2(int x=2) : v(x) {}
    void c2(int c) { if( v != c ) fail(__LINE__); }
};
struct V3 {
    int v;
    V3(int x=3) : v(x) {}
    void c3(int c) { if( v != c ) fail(__LINE__); }
};
struct V : virtual V1, virtual V2, virtual V3 {
    int v;
    V() : v(4), V1(-1), V2(-2), V3(-3) {}
};

void test( va_list args, unsigned line )
{
    int c;
    int x;

    c = 1;
    for(;;) {
	x = va_arg( args, int );
	if( x == 0 ) break;
	if( x != c ) fail( line );
	++c;
    }
    va_end( args );
}

struct B {
    V1 v1;
    V2 v2;
    V3 v3;
    virtual V1 &f1(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v1;
    }
    virtual V2 &f2(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v2;
    }
    virtual V3 &f3(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v3;
    }
};
struct D : B {
    V v;
    virtual V &f1(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v;
    }
    virtual V &f2(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v;
    }
    virtual V &f3(int c, ...) {
	va_list args;

	va_start( args, c );
	test( args, __LINE__ );
	if( c != -3 ) fail(__LINE__); return v;
    }
};

void test( B &r, D &q )
{
    (r.f1(-3,0)).c1(V1_CHECK);
    (r.f2(-3,1,0)).c2(V2_CHECK);
    (r.f3(-3,1,2,0)).c3(V3_CHECK);
    (q.f1(-3,1,2,3,0)).c1(-1);
    (q.f2(-3,1,2,3,4,0)).c2(-2);
    (q.f3(-3,1,2,3,4,5,0)).c3(-3);
}

int main()
{
    D x;
    test( x, x );
    _PASS;
}
