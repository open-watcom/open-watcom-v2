#include "fail.h"

int x;
void *p;

struct A0 {
    int a[10];
    virtual int f0() { return __LINE__; }
    virtual int f1() { return __LINE__; }
    virtual int f2() { return __LINE__; }
    virtual int f3() { return __LINE__; }
    virtual int f4() { return __LINE__; }
    virtual int f5() { return __LINE__; }
    virtual int f6() { return __LINE__; }
    virtual int f7() { return __LINE__; }
    virtual int f8() { return __LINE__; }
};

struct B0 {
    int a[10];
    virtual operator int ();
};
B0::operator int() {
    if( this != p ) fail(__LINE__);
    return x + 1;
}

struct BB : B0 {
    int a[10];
    int inc;
};
struct B : BB {
    int a[10];
    operator char(){ return __LINE__; }
    virtual operator float() { return __LINE__; }
};

struct D1 : B {
    int a[10];
    virtual operator int ();
};
D1::operator int() {
    if( this != p ) fail(__LINE__);
    return x + 2;
}

struct DD1 : A0, D1 {
    int a[10];
};

struct D2 : B {
    int a[10];
    virtual operator int ();
};
D2::operator int() {
    if( this != p ) fail(__LINE__);
    return x + 3;
}

struct D3 : virtual B {
    int a[10];
    virtual operator int ();
};
D3::operator int() {
    if( this != p ) fail(__LINE__);
    return x + 4;
}

struct E : D1, D2 {
    int a[10];
};

int inline transform( B0 &r, int i, void *value_p ) {
    p = value_p;
    x = i;
    return r.operator int();
}

int main()
{
    B b;
    D1 d1;
    D2 d2;
    D3 d3;
    DD1 dd1;
    E e;

    if( transform( b, 1, (B0*)&b ) != 2 ) fail(__LINE__);
    if( transform( d1, 1, (D1*)&d1 ) != 3 ) fail(__LINE__);
    if( transform( d2, 1, (D2*)&d2 ) != 4 ) fail(__LINE__);
    if( transform( d3, 1, (D3*)&d3 ) != 5 ) fail(__LINE__);
    if( transform( dd1, 1, (D1*)&dd1 ) != 3 ) fail(__LINE__);
    if( transform( (D1 &)e, 1, (D1*)&e ) != 3 ) fail(__LINE__);
    if( transform( (D2 &)e, 1, (D2*)&e ) != 4 ) fail(__LINE__);
    _PASS;
}
