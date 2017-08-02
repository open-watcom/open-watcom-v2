#include "fail.h"

// verify that return value thunks don't occupy the same slot in vftable
int sign;

struct V1 {
    int v;
    V1(int x=1) : v(x) {}
    void c1() { if( v != sign*1 ) fail(__LINE__); }
};
struct V2 {
    int v;
    V2(int x=2) : v(x) {}
    void c2() { if( v != sign*2 ) fail(__LINE__); }
};
struct V3 {
    int v;
    V3(int x=3) : v(x) {}
    void c3() { if( v != sign*3 ) fail(__LINE__); }
};
struct V : virtual V1, virtual V2, virtual V3 {
    int v;
    V() : v(4), V1(-1), V2(-2), V3(-3) {}
};

struct B {
    V1 v1;
    V2 v2;
    V3 v3;
    virtual V1 &f1();
    virtual V2 &f2();
    virtual V3 &f3();
    B();
};
B::B() {
}
V1 &B::f1() { return v1; }
V2 &B::f2() { return v2; }
V3 &B::f3() { return v3; }
struct D : B {
    V v;
    virtual V &f1();
    virtual V &f2();
    virtual V &f3();
    D();
};
D::D() {
}
V &D::f1() { return v; }
V &D::f2() { return v; }
V &D::f3() { return v; }

void test( B &r, D &q, int s1, int s2 )
{
    sign = s1;
    (r.f1()).c1();
    (r.f2()).c2();
    (r.f3()).c3();
    sign = s2;
    (q.f1()).c1();
    (q.f2()).c2();
    (q.f3()).c3();
}

int main()
{
    D x;
    test( x, x, -1, -1 );
    B y;
    test( y, x, 1, -1 );
    _PASS;
}
