/*
 * OW 1.7 failure in &
 */

#include "fail.h"

int test_function( long int a )
{
    return( (a & 0xffff0001) != 0xffff0001);
}

/* Test constant folding in cg; inlined functions make it highly likely 
 * that the front end won't be able to perform folding, but cg will.
 */
long long __inline rshift_i( long long arg )
{
    return( arg >> 5 );
}

long long __inline lshift_i( long long arg )
{
    return( arg << 3 );
}

long long __inline div_i( long long arg )
{
    return( arg / (24 * 3600L) );
}

long long __inline mod_i( long long arg )
{
    return( arg % (24 * 3600L) );
}

double __inline div_f( double arg )
{
    return( arg / 4 );
}

int main( void )
{
    if( test_function( 0xffffffff ) ) fail( __LINE__ );
    if( rshift_i( 256 ) != 8 ) fail( __LINE__ );
    if( rshift_i( -256 ) != -8 ) fail( __LINE__ );
    if( lshift_i( 3 ) != 24 ) fail( __LINE__ );
    if( lshift_i( -3 ) != -24 ) fail( __LINE__ );
    if( div_i( -1 ) != 0 ) fail( __LINE__ );
    if( div_i( 100000 ) != 1 ) fail( __LINE__ );
    if( mod_i( -1 ) != -1 ) fail( __LINE__ );
    if( mod_i( -100000 ) != -13600 ) fail( __LINE__ );
    if( div_f( -1 ) != -0.25 ) fail( __LINE__ );
    if( div_f( 10 ) != 2.5 ) fail( __LINE__ );
    _PASS;
}
