// 11.0
//		works? _=yes
// -d2i -xst	_
// -d2i -xs 	_
// -d2i -xss	_
// -d2  -xst	N
// -d2  -xs 	_
// -d2  -xss	_
// -d1  -xst	N
// -d1  -xs 	_
// -d1  -xss	_
#include "fail.h"
#include <string.h>

unsigned s_dtor;

struct S {
    S *next;
    static S *freed;
    void *operator new( unsigned s )
    {
	return new char[s];
    }
    void operator delete( void *p ) {
	S *t = (S*)p;
	for( S *c = freed; c; c = c->next ) {
	    if( c == t ) fail(__LINE__);
	}
	t->next = freed;
	freed = t;
    }
    S( int x ) : x(x) {
    }
    int x;
    ~S() {
	++s_dtor;
    }
    void zap() {
	memset( this, -1, sizeof(*this) );
    }
};
S *S::freed;

unsigned once;

struct L {
    S *h;
    ~L() {
	S *n;
	if( once++ == 0 ) {
	    for( S *c = h; c; c = n ) {
		n = c->next;
		c->zap();
		delete c;
		throw 1;	// h will point to freed memory!
	    }
	} else {
	    // should only be called once
	    fail(__LINE__);
	}
    }
    void add( int v ) {
	S *p = new S(v);
	p->next = h;
	h = p;
    }
    L() : h(0) {
    }
};

void test() {
    L xx;

    xx.add(1);
    xx.add(2);
    xx.add(3);
}

int main() {
    static int ok;
    try {
	test();
    } catch( ... ) {
	++ok;
    }
    if( s_dtor != 1 ) fail(__LINE__);
    if( !ok ) fail(__LINE__);
    _PASS;
}
