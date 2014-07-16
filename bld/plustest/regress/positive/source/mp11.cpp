#include "fail.h"

struct B1 {
    int b1;
};
struct B2 {
    int b2;
};
struct D : B1, B2 {
    int f1( D * );
    int f2( D * );
};

typedef int (B1::* mp1)( void * );
typedef int (B2::* mp2)( void * );

void *t;

int launch1( B1 *p, mp1 mp ) {
    return (p->*mp)( (void*)1 );
}
int launch2( B2 *p, mp2 mp ) {
    return (p->*mp)( (void*)2 );
}
int D::f1( D *p ) {
    if( this != t ) _fail;
    return (int) p;
}
int D::f2( D *p ) {
    if( this != t ) _fail;
    return (int) p;
}

int main() {
    D x;
    t = &x;
    if( launch1( &x, (mp1) &D::f1 ) != 1 ) _fail;
    if( launch2( &x, (mp2) &D::f2 ) != 2 ) _fail;
    _PASS;
}
