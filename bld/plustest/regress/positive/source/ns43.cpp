#include "fail.h"

namespace ns1
{
    struct A
    { };
}

namespace ns2
{
    using namespace ns1;

    template< class T >
    struct B
    {
        A a;

        void f1()
        {
	    A a;
        }

        void f2();
    };

    template< class T >
    void B<T>::f2()
    {
        A a;
    };

    template< class T >
    void g(T t)
    {
        A a;
    };
}


template< class T >
struct D {
    struct D2 {
        int fnd2();
    };

    int static fnd()
    { return 1; }
};

template< class T >
int D<T>::D2::fnd2()
{
     return fnd();
}


int main()
{
    ns2::B< int > b;

    b.f1();
    b.f2();

    ns2::g(0);


    D< int >::D2 d2;

    if( d2.fnd2() != 1 ) fail(__LINE__);


    _PASS;
}
