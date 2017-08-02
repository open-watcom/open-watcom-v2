#include "fail.h"

extern int _Optlink foo( int p1, int p2, int p3, int p4, int p5 )
{
    return( p1 + p2 + p3 + p4 + p5 );
}

float _Optlink bar( int p1, float p2, float p3, int p4, int p5, float p6, float p7, int p8, int p9, float const *p10 )
{
    return( *(p10 + 1) + p8 + p6 + p2 );
}

double _Optlink baz( int p1, double p2, double p3, int p4, int p5, double p6, double p7, int p8, int p9, float const *p10 )
{
    return( *(p10 + 1) + p8 + p6 + p2 );
}

#define xbit( i, b )    ((i>>b)&1)

float a[] = { 10, 11 };

int main( void )
{
    int     s = 0, i;
    float   f;
    double  g;

    for( i = 0; i < 10; ++i ) {
        s += foo( xbit(i,0), xbit(i,1), xbit(i,2), xbit(i,3), xbit(i,4) );
    }
    if( s != 15 ) fail( __LINE__ );

    f = bar( 1, 2, 3, 4, 5, 6, 7, 8, 9, a );
    if( f != 27.0f ) fail( __LINE__ );

    g = baz( 1, 2, 3, 4, 5, 6, 7, 8, 9, a );
    if( f != 27.0 ) fail( __LINE__ );

    _PASS;
}
