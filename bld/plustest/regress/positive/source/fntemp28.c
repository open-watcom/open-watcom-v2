#include "fail.h"

template< int N1, int N2 >
bool f( int n1, int n2 ) {
    return ( N1 == n1 ) && ( N2 == n2 );
}

const int C1 = 5;
const int C2 = 6;

template< int N >
struct A {
    static const int C = N;
};

int main()
{
    if( ! f< 1, 2 >( 1, 2 ) ) fail( __LINE__ );
    if( ! f< 2, 1 >( 2, 1 ) ) fail( __LINE__ );

    if( ! f< C1, C2 >( C1, C2 ) ) fail( __LINE__ );
    if( ! f< C2, C1 >( C2, C1 ) ) fail( __LINE__ );

    if( ! f< A< C1 >::C, A< C2 >::C >( C1, C2 ) ) fail( __LINE__ );

    _PASS;
}
