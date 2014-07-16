// this code will not work if the CFRONT object model is used
#include "fail.h"
#include <string.h>

struct V {
    virtual void set( int x ) { v = x; }
    virtual int get() { return v; }
    int v;
    V() {
	set( 'v' );
	if( get() != 'v' ) fail(__LINE__);
    }
};

void unknown_set( V *p, char x )
{
    p->set( x );
}

struct D : virtual V {
    virtual void set( int x ) { d = x; }
    virtual int get() { return d; }
    int d;
    D() {
	if( v != 'v' ) fail(__LINE__);
	set( 'd' );
	if( get() != 'd' ) fail(__LINE__);
	V *p = this;
	unknown_set( p, 'D' );
	if( get() != 'D' ) fail(__LINE__);
	if( p->get() != 'D' ) fail(__LINE__);
    }
};

struct E : virtual V {
    virtual void set( int x ) { e = x; }
    virtual int get() { return e; }
    int e;
    E() {
	if( v != 'v' ) fail(__LINE__);
	set( 'e' );
	if( get() != 'e' ) fail(__LINE__);
	V *p = this;
	unknown_set( p, 'E' );
	if( get() != 'E' ) fail(__LINE__);
	if( p->get() != 'E' ) fail(__LINE__);
    }
};
struct V1 {
    int v1[10];
};
struct V2 {
    int v2[10];
};
struct DD : virtual V1, E, virtual V, D, virtual V2 {
    int dd;
    virtual void set( int x ) { dd = x; }
    virtual int get() { return dd; }
    void * operator new( size_t size )
    {
	char *p = new char[size];
	memset( p, -1, size );
	return p;
    }
};

int main()
{
    DD *p = new DD;
    if( p->v != 'v' ) fail(__LINE__);
    if( p->d != 'D' ) fail(__LINE__);
    if( p->e != 'E' ) fail(__LINE__);
    if( p->dd != -1 ) fail(__LINE__);
    for( int i = 0; i < 10; ++i ) {
	if( p->v1[i] != -1 ) fail(__LINE__);
	if( p->v2[i] != -1 ) fail(__LINE__);
    }
    delete p;
    _PASS;
}
