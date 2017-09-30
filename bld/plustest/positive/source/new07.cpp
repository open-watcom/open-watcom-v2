#include "fail.h"

unsigned dtor_called;
unsigned delete_called;

char buff[100];

struct X {
    int x;
    X(int v) : x(v) {}
};

struct T {
    ~T() { ++dtor_called; }
    void *operator new( unsigned )
    {
	return buff;
    }
    void operator delete( void *p )
    {
	if( p != buff ) fail(__LINE__);
	++delete_called;
    }
};

struct S : T {
    int x,y;
    S( int x, int y ) : x(x), y(y) { throw X(1); }
};

S *foo()
{
    return new S(3,4);
}

int main()
{
    try {
	foo();
    } catch( X &r ) {
	if( r.x != 1 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( delete_called != 1 ) fail(__LINE__);
    if( dtor_called != 1 ) fail(__LINE__);
    _PASS;
}
