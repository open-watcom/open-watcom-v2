#include "fail.h"

struct A { int a; };
struct C { int c; };
struct B : public C, public A { int b; };

typedef A * T;

T check;

struct Array {
    void Append( const T & t ) { if( t != check ) _fail; }
    void Remove( const T & t ) { if( t != check ) _fail; }
};

void Foo( B * b )
{
    Array       arr;
    A *         a = b;
    arr.Append( a );
    arr.Remove( b );
}

int main()
{
    B   b;
    check = &b;
    Foo( &b );
    _PASS;
}
