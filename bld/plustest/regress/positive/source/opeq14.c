#include "fail.h"

unsigned a_called;
unsigned b_called;

struct A {
    int &r;
    A( int * );
    A & operator =( A const &s );
};
struct B {
    int r;
    B & operator =( B const &s );
    B( int v ) : r(v) {
    }
};
struct C {
    A a;
    B b;
    C( int *p ) : a(p), b(0) {
    }
};

void foo( C x, C y )
{
    x = y;
    y = x;
}

A::A( int *p ) : r(*p)
{
}
A & A::operator =( A const &s ) {
    ++a_called;
    return *this;
}
B & B::operator =( B const &s ) {
    ++b_called;
    return *this;
}

int main()
{
    int z = 3;
    C x( &z );
    C y( &z );
    foo( x, y );
    if( a_called != 2 || b_called != 2 ) _fail;
    _PASS;
}
