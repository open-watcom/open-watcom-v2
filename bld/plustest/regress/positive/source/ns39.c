#include "fail.h"

namespace ns {
template< class T >
struct A {
};

template< class T >
A< T > *f( const T &x ) {
    return 0;
}
}

int main( ) {
    int i = 1;

    ns::f( i );
    ns::f( i );

    _PASS;
}
