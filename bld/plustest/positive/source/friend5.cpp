// test interaction of friends and template instantiations
#include "fail.h"

template< class T >
class A {
    friend void f( A< void > * );

    int i;
};

template< class T >
class B {
    friend void f( B< void > * );

    int i;
};

void f( A< void > *a )
{
    void *ptr = &(a->i);
}

void f( B< void > *b )
{
    void *ptr = &(b->i);
}


template< class T >
class C {
    T t;

public:
    void f() {
        D d;
        t = d.i;
    }
};

class D {
    friend class C< void >; // shouldn't cause C< void > to be instantiated
    friend class C< int >;

    template< class T >
    friend void g( T t );

    int i;
};

template< class T >
void g( T t )
{
    D d;
    d.i = 0;
}


int main() {
    A< void > *a = 0;
    B< void > *b = 0;

    f( a );
    f( b );

    C< int > c;
    c.f();

    g( 0 );


    _PASS;
}
