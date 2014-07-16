#include "fail.h"
#include <stdio.h>
#include <time.h>

struct x0 {
    static int count;
    x0() { ++count; }
    x0( x0 const & ) { ++count; }
    ~x0() { --count; }
};

struct x1 : x0 {
    static int count;
    int v;
    x1( int v ) : v(v) { ++count; }
    x1( x1 const &s ) : v(s.v) { ++count; }
    ~x1() { --count; }
};

struct x2 : x0 {
    static int count;
    int v;
    x2( int v ) : v(v) { ++count; }
    x2( x2 const &s ) : v(s.v) { ++count; }
    ~x2() { --count; }
};

int x0::count;
int x1::count;
int x2::count;

unsigned count;

struct cd {
    cd() { ++count; }
    ~cd() { --count; }
    void dies() {
	throw x2(__LINE__);
    }
};

void f1( int v )
{
    cd *p;

    try {
	p = new cd;
	p->dies();
    } catch( x0 & ) {
	delete p;
	throw x1(v);
    } catch( ... ) {
	fail( __LINE__ );
    }
}

int main()
{
    int i = 0;
    clock_t start;
    clock_t stop;
    clock_t check;

    start = clock();
    stop = start + CLK_TCK * 10;
    for(;;) {
	try {
	    f1( i );
	} catch( x1 &r ) {
	    if( r.v != i ) fail(__LINE__);
	} catch( x0 &r ) {
	    fail( __LINE__ );
	} catch( ... ) {
	    fail( __LINE__ );
	}
	if( errors ) break;
	check = clock();
	if( check > stop ) {
	    break;
	}
	++i;
    }
    if( x0::count != 0 ) fail(__LINE__);
    if( x1::count != 0 ) fail(__LINE__);
    if( x2::count != 0 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    _PASS;
}
