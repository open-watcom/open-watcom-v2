#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "fail.h"

int res[5];

double csin( double g, long h )
{
    h = labs( h );
    return( sin( g ) );
}

void orderInt( char p1, short p2, int p3, long p4, long long p5 )
{
    res[0] = p1;
    res[1] = p2;
    res[2] = p3;
    res[3] = p4;
    res[4] = p5;
}

void orderFlt( float f1, double f2, long double f3, float f4, double f5 )
{
    res[0] = f1;
    res[1] = f2;
    res[2] = f3;
    res[3] = f4;
    res[4] = f5;
}

int main( void )
{
    orderInt( 1, 2, 3, 4, 5 );
    if( res[0] != 1 ) fail( __LINE__ );
    if( res[1] != 2 ) fail( __LINE__ );
    if( res[2] != 3 ) fail( __LINE__ );
    if( res[3] != 4 ) fail( __LINE__ );
    if( res[4] != 5 ) fail( __LINE__ );
    orderFlt( 1.0, 2.0, 3.0, 4.0, 5.0 );
    if( res[0] != 1 ) fail( __LINE__ );
    if( res[1] != 2 ) fail( __LINE__ );
    if( res[2] != 3 ) fail( __LINE__ );
    if( res[3] != 4 ) fail( __LINE__ );
    if( res[4] != 5 ) fail( __LINE__ );
    _PASS;
}

#ifdef __cplusplus
}
#endif
