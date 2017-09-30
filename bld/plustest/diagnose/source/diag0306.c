template< class T >
struct A {
    static const int val = sizeof( T );

    static_assert( val == sizeof( char ), "message 1" );
};

static_assert( false, "message 2" );

namespace ns {
    static_assert( 1 + 1 != 2, "message 3" );
}

struct B {
    static_assert( false, "message 4" );
};

int main() {
    static_assert( A< char >::val == sizeof( int ), "message 5" );
    A< int > a;

    return 0;
}
