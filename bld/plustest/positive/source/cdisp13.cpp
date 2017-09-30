#include "fail.h"

#ifdef __WATCOM_RTTI__

struct V {
    virtual int pass( int x ) {
	return x;
    }
    V() {
    }
    int v[10];
};

struct D;
D *V2D( V *p );

struct D : virtual V {
    D() {
	V *pv = this;
	D *pd = V2D( pv );
	if( pd != this ) fail(__LINE__);
    }
    int d[10];
};

struct E;
E *V2E( V *p );

struct E : virtual V {
    E() {
	V *pv = this;
	E *pe = V2E( pv );
	if( pe != this ) fail(__LINE__);
    }
    int e[10];
};

D *V2D( V *p )
{
    return dynamic_cast<D*>( p );
}

E *V2E( V *p )
{
    return dynamic_cast<E*>( p );
}

struct F : virtual D, virtual E {
    F() {
    }
    int f[10];
};

int main() {
    D *pd = new D();
    delete pd;
    E *pe = new E();
    delete pe;
    F *pf = new F();
    delete pf;
    _PASS;
}

#else

ALWAYS_PASS

#endif
