#include "fail.h"

int count;

void die()
{
    if( --count <= 0 ) throw 1;
}

int ctors;

struct S {
    int v;
    S() { ++ctors; die(); }
    S(int x) : v(x) { ++ctors; die(); }
    ~S() { --ctors; if( ctors < 0 ) fail(__LINE__); }
};

void test( int n )
{
    count = n;
    S xx[5] = { 1, 2 };
}

void foo()
{
    for( int i = 0; i < 10; ++i ) {
	try {
	    test( i );
	} catch( int x ) {
	    if( x != 1 ) fail(__LINE__);
	} catch( ... ) {
	    fail(__LINE__);
	}
    }
}

int main()
{
    foo();
    _PASS;
}
