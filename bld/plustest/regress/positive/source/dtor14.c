#include "fail.h"

int v = -1000 - 10;

unsigned ctors;
unsigned dtors;

struct A {
    A( int n = 10, int k = ( v += 10 ) ) {
	++ctors;
	_p = new int[n];
	for( int i = 0; i < n; ++i ) {
	    _p[i] = k++;
	}
    }
    ~A() { ++dtors; delete [] _p; }
    int *_p;
};

struct S {
    S();
    ~S();
    A _a[10];
};

S::S() {
}

S::~S() {
}

int main()
{
    {
	S v;
	int c = -1000;

	if( ctors != 10 ) fail(__LINE__);
	for( int i = 0; i < 10; ++i ) {
	    for( int j = 0; j < 10; ++j ) {
		if( v._a[i]._p[j] != c ) fail(__LINE__);
		++c;
	    }
	}
    }
    if( dtors != 10 ) fail(__LINE__);
    _PASS;
}
