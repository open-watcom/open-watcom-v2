#include "fail.h"


struct A{
    struct B1{
        B1() {}
    };

    struct B2{
        B2(){}
        B2( B1 ) {}
    };

    template< class T >
    void fn( B2, T ) {}

    template< class T >
    void fn( int, T ) {}

    template< class T >
    void fn( short, T ) {}
};


struct B {
    template< class T >
    explicit B( T t );

    ~B()
    { }

    template< class T >
    void f( T t );
};

template< class T >
B::B( T t )
{ }

template< class T >
void B::f( T t )
{ }


void g( B &b )
{
    B bb = b;
}


int main(void)
{
    A a;
    A::B1 b1;
    A::B2 b2;

    a.fn( b1, 1 );
    a.fn( b2, 1 );


    B b( 1 );
    b.f( 2 );


    _PASS;
}
