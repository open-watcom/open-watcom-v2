#include "fail.h"

struct V1 {
    int v1;
    V1() : v1(1) {
    }
};
struct V2 {
    int v2;
    V2() : v2(2) {
    }
};
struct V3 {
    int v3;
    V3() : v3(3) {
    }
};
struct DV1 : virtual V1 {
    int dv1;
    DV1() : dv1(4) {
    }
};
struct DV2 : virtual V2 {
    int dv2;
    DV2() : dv2(5) {
    }
};
struct DV3 : virtual V3 {
    int dv3;
    DV3() : dv3(6) {
    }
};
struct DV : DV1, DV2, DV3 {
    int dv;
    DV() : dv(7) {
	++v1;
	++v2;
	++v3;
    }
};

struct B1 {
    virtual V1 *f();
    virtual V1 *f1();
};
struct B2 {
    virtual V2 *f();
    virtual V2 *f2();
};
struct B3 {
    virtual V3 *f();
    virtual V3 *f3();
};
struct D : B1, B2, B3 {
    virtual DV *f();
};

V1 *B1::f() {
    return new V1;
}
V2 *B2::f() {
    return new V2;
}
V3 *B3::f() {
    return new V3;
}
V1 *B1::f1() {
    return new V1;
}
V2 *B2::f2() {
    return new V2;
}
V3 *B3::f3() {
    return new V3;
}
DV *D::f() {
    return new DV;
}

D x;

void c1( V1 *p, int a ) {
    if( p->v1 != (1+a) ) fail(__LINE__);
}
void c2( V2 *p, int a ) {
    if( p->v2 != (2+a) ) fail(__LINE__);
}
void c3( V3 *p, int a ) {
    if( p->v3 != (3+a) ) fail(__LINE__);
}
void cd( DV *p, int a ) {
    if( p->v1 != (1+a) ) fail(__LINE__);
    if( p->v2 != (2+a) ) fail(__LINE__);
    if( p->v3 != (3+a) ) fail(__LINE__);
    if( p->dv1 != 4 ) fail(__LINE__);
    if( p->dv2 != 5 ) fail(__LINE__);
    if( p->dv3 != 6 ) fail(__LINE__);
    if( p->dv != 7 ) fail(__LINE__);
}
void bc1( B1 *p ) {
    c1( p->f(), 1 );
    c1( p->f1(), 0 );
}
void bc2( B2 *p ) {
    c2( p->f(), 1 );
    c2( p->f2(), 0 );
}
void bc3( B3 *p ) {
    c3( p->f(), 1 );
    c3( p->f3(), 0 );
}
void bcd( D *p ) {
    c1( p->f(), 1 );
    c1( p->f1(), 0 );
    c2( p->f(), 1 );
    c2( p->f2(), 0 );
    c3( p->f(), 1 );
    c3( p->f3(), 0 );
    cd( p->f(), 1 );
}

int main() {
    bc1( &x );
    bc2( &x );
    bc3( &x );
    bcd( &x );
    _PASS;
}
