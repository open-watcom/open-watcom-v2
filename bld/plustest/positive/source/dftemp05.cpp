#include "fail.h"

template< class T >
struct A {
    static const int val = 0;
};

template< class T = A< int > >
struct B {
    static const int val = sizeof(T);
};

template< int n = (0 < 1 > 0) >
struct C {
};

template< int n = A< int(char[3 > 2]) >::val >
struct D {
};

template< class T = char[2 > 0] >
struct E {
};

template< int n = (1 > 2), int m = (1 < 2) >
struct F {
};

int main()
{
    A< int > a;

    B<> b1;
    B< A< char > > b2;

    C<> c1;
    C< 0 > c2;

    D<> d1;
    D< 0 > d2;

    E<> e1;
    E< char[2 > 1] > e2;

    F<> f1;
    F< 0 > f2;

    _PASS;
}
