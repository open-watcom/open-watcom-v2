#include "fail.h"

template< class T >
struct A
{
    static int f()
    {
        return 1;
    }
};

struct B
    : A< int >
{
    static int f()
    {
        // not valid in C++ 98, but valid in later versions (see 14.6.1 (1))
        return 4 + B::g() + A::f() + A<int>::f();
    }

    static int g()
    {
        return 2;
    }
};


int main()
{
    if( B::f() != 8 ) fail( __LINE__ );

    _PASS;
}
