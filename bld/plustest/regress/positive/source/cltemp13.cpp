#include "fail.h"

template< typename T >
struct A {
    typedef T D;
};

template< typename C >
struct B {
    typename C::D t;

    typename C::D f() {
        return typename C::D();
    }
};


template< class T >
struct C {
    typedef C CC;

    void f() {
        typename C< T >::CC *c = 0;
    }
};

struct D {
    C< int > c;
};


int main()
{
    B< A< int > > b;

    int &i = b.t;
    i = b.f();

    D d;
    d.c.f();


    _PASS;
}
