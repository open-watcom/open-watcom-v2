#include "fail.h"
#include <math.h>
#include <float.h>

/* Make sure the type of the folded floating-point constant corresponds
 * to the type of the expression. (Bug 916)
 */

int f_f( void )
{
    return( fpclassify( 1.f + 1.f ) );
}

int f_d( void )
{
    return( fpclassify( 1.f + 1. ) );
}

int d_f( void )
{
    return( fpclassify( 1. + 1.f ) );
}

int f_i( void )
{
    return( fpclassify( 1.f + 1 ) );
}

int i_f( void )
{
    return( fpclassify( 1 + 1.f ) );
}

int d_i( void )
{
    return( fpclassify( 1. + 1 ) );
}

int i_d( void )
{
    return( fpclassify( 1 + 1. ) );
}

int u_f( void )
{
    return( fpclassify( - 1.f ) );
}

int u_d( void )
{
    return( fpclassify( - 1. ) );
}

int f_f_f( void )
{
    return( fpclassify( 1.f - 2.f + 1.f ) );
}

int d_d_d( void )
{
    return( fpclassify( 1. - 2. + 1. ) );
}

/* Test comparisons against constants. Note that INFINITY is always float
 * type, but may be converted to (long) double. (Bug 925)
 */
int cmpinf( void )
{
    if( !(FLT_MAX < INFINITY) )
        return( 1 );
    if( !(DBL_MAX < INFINITY) )
        return( 2 );
    if( !(LDBL_MAX < INFINITY) )
        return( 3 );
    return( 0 );
}

int main( void )
{
    if( f_f() != FP_NORMAL ) fail( __LINE__ );
    if( f_d() != FP_NORMAL ) fail( __LINE__ );
    if( d_f() != FP_NORMAL ) fail( __LINE__ );
    if( f_i() != FP_NORMAL ) fail( __LINE__ );
    if( i_f() != FP_NORMAL ) fail( __LINE__ );
    if( f_i() != FP_NORMAL ) fail( __LINE__ );
    if( i_d() != FP_NORMAL ) fail( __LINE__ );
    if( d_i() != FP_NORMAL ) fail( __LINE__ );
    if( u_f() != FP_NORMAL ) fail( __LINE__ );
    if( u_d() != FP_NORMAL ) fail( __LINE__ );
    if( f_f_f() != FP_ZERO ) fail( __LINE__ );
    if( d_d_d() != FP_ZERO ) fail( __LINE__ );
    if( cmpinf() ) fail( __LINE__ );

    _PASS;
}
