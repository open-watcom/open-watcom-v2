#include "fail.h"

namespace ns {
    struct B {
        int f();
    };

    int B;

    namespace ns2 {
        template< class T >
        int f( T t )
        {
            return B + 4;
        }
    }
}

int ns::B::f()
{
    return ns::B + 3;
}

int main() {
    ::ns::B = 3;

    struct ::ns::B b1;
    if( b1.f() != 6 ) fail( __LINE__ );


    ns::B = 5;

    struct ns::B b2;
    if( b2.f() != 8 ) fail( __LINE__ );


    if( ns::ns2::f< int >( 1 ) != 9 ) fail( __LINE__ );


    _PASS;
}
