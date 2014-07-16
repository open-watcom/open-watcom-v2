// b::op del isn't generated!!!
#include "fail.h"

#pragma inline_depth(0)
int x;
struct B {
    void *operator new( unsigned ){++x;return 0;};
    void operator delete( void *, unsigned ){++x;};
    virtual ~B(){++x;};
};
struct D : B {
    void *operator new( unsigned ){++x;return 0;}
    void operator delete( void *, unsigned ){++x;}
};
struct E : B {
    void *operator new( unsigned ){++x;return 0;}
    void operator delete( void *, unsigned ){++x;}
    ~E() {
	++x;
    }
};
struct F : B {
    void *operator new( unsigned ){++x;return 0;}
    void operator delete( void *, unsigned ){++x;}
    virtual ~F() {
	++x;
    }
};

int main() {
    new D;
    new E;
    new F;
    new D[3];
    new E[3];
    new F[3];
    {D x;}
    {E x;}
    {F x;}
    {D x[2];}
    {F x[2];}
    {E x[2];}
    _PASS;
}
