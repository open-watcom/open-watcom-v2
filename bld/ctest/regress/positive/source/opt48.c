/*
 * Test conditionals optimization. Old compilers missed the fact that
 * the 'f2 = f1' assignment modifies f2 and hence the result of the second
 * 'if' may not be reused in the third 'if' statement. This was because the
 * double (FD) is written as two dwords (I4) and the overlap was not
 * recognized. 
 */

#include "fail.h"

int test_function( double *dbl )
{
    double  f1  = 1.0;
    double  f2  = 0.0;

    if( dbl == NULL )
        ++dbl;

    if( (f2 = *dbl) < 0.0 )
        f2 = f1;

    if( f2 >= 0.0 )
        return( 0 );
    else
        return( 1 );
}

int main( void )
{
    double  dbl;

    dbl = -1.0;
    if( test_function( &dbl ) ) fail( __LINE__ );
    _PASS;
}
