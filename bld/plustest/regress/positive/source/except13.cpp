#include "fail.h"

int alloc_count;

struct S {
    S( int x ) : x(x) { throw int(x); }
    ~S() { fail(__LINE__); }
    void *operator new( unsigned x ) {
	++alloc_count;
	return malloc( x );
    }
    void operator delete( void *p ) {
	--alloc_count;
	if( alloc_count < 0 ) fail(__LINE__);
	free( p );
    }
    int x;
};

void test( int v )
{
    try {
	S *p = new S(v);
	delete p;
    } catch( int x ) {
	if( x != v ) fail(__LINE__);
	return;
    } catch( ... ) {
	fail(__LINE__);
    }
}

int main()
{
    for( int i = 0; i < 10000; ++i ) {
	test( i );
	if( alloc_count != 0 ) {
	    fail(__LINE__);
	    break;
	}
	if( errors ) break;
    }
    _PASS;
}
