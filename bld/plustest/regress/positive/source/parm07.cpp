#include "fail.h"

#pragma inline_depth(0);

struct __cdecl V {
    int v;
    virtual ~V();
    V();
    V( const V & );
    V & operator =( V const & );
};

int ctors;

V::V() : v(++ctors) {
}

V::V( V const & ) : v(++ctors) {
}

V::~V() {
    --ctors;
    if( ctors < 0 ) fail(__LINE__);
}

V & V::operator =( V const &s ) {
    v = s.v;
    return *this;
}

struct __cdecl B : virtual V {
    virtual ~B() {};
};
struct __cdecl C : virtual V {
    virtual ~C() {};
};

struct __cdecl D : B, C, virtual V {
    virtual void foo(){};
    void *operator new( unsigned s )
    {
	void *p = malloc( sizeof( void * ) + s + sizeof( void * ) );
	void **b = (void**) p;
	void **e = (void**) (((char*)p) + sizeof( void * ) + s);
	*b = e;
	*e = b;
	return (void*) (((char*)p) + sizeof( void * ));
    }
    void operator delete( void *p, unsigned s )
    {
	void **b = (void**) (((char*)p) - sizeof( void * ));
	void **e = (void**) (((char*)b) + sizeof( void * ) + s);
	if( *b != e ) fail(__LINE__);
	if( *e != b ) fail(__LINE__);
	free( b );
    }
};

int main()
{
    D *p = new D;
    D *q = new D;
    *q = *p;
    V *r = (C*) p;
    delete r;
    if( q->v != 1 ) fail(__LINE__);
    B *s = q;
    delete s;
    _PASS;
}
