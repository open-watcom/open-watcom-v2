#include "fail.h"

// verify that return value thunks don't occupy the same slot in vftable

struct V1 {
    int v;
    V1(int x=1) : v(x) {}
    void c1() { if( v != -1 ) fail(__LINE__); }
};
struct V2 {
    int v;
    V2(int x=2) : v(x) {}
    void c2() { if( v != -2 ) fail(__LINE__); }
};
struct V3 {
    int v;
    V3(int x=3) : v(x) {}
    void c3() { if( v != -3 ) fail(__LINE__); }
};
struct V : virtual V1, virtual V2, virtual V3 {
    int v;
    V() : v(4), V1(-1), V2(-2), V3(-3) {}
};

struct B {
    V1 v1;
    V2 v2;
    V3 v3;
    virtual V1 &f1() { return v1; }
    virtual V2 &f2() { return v2; }
    virtual V3 &f3() { return v3; }
};
struct D : B {
    V v;
    virtual V &f1() { return v; }
    virtual V &f2() { return v; }
    virtual V &f3() { return v; }
};

void test( B &r, D &q )
{
    (r.f1()).c1();
    (r.f2()).c2();
    (r.f3()).c3();
    (q.f1()).c1();
    (q.f2()).c2();
    (q.f3()).c3();
}

int main()
{
    D x;
    test( x, x );
    _PASS;
}
