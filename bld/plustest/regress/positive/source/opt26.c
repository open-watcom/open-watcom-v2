#include "fail.h"



#pragma inline_depth(0)
int operations;
struct B {
    void *operator new( unsigned ){++operations;return 0;};
    void operator delete( void *, unsigned ){++operations;};
    virtual ~B(){++operations;};
};
struct D : B {
    void *operator new( unsigned ){++operations;return 0;}
    void operator delete( void *, unsigned ){++operations;}
};
struct E : B {
    void *operator new( unsigned ){++operations;return 0;}
    void operator delete( void *, unsigned ){++operations;}
    ~E() {
        ++operations;
    }
};
struct F : B {
    void *operator new( unsigned ){++operations;return 0;}
    void operator delete( void *, unsigned ){++operations;}
    virtual ~F() {
        ++operations;
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
    if( operations != 18 ) fail( __LINE__ );
    _PASS;
}
