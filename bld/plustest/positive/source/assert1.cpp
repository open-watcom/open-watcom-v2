#include "fail.h"

template< class T >
struct A {
    static const int val = sizeof( T );

    static_assert( val != 0, "message" );
};

static_assert( true, "message" );

namespace ns {
    static_assert( 1 + 1 == 2, "message" );
}

struct B {
    static_assert( true, "message" );
};

int main() {
    static_assert( A< char >::val == sizeof( char ), "message" );

    _PASS;
}
