#include "fail.h"
#include <float.h>
#include <math.h>

double d = DBL_MAX;
double e = -DBL_MAX;
double f = DBL_MAX*DBL_EPSILON;

/* Test long long integer to floating-point conversion */
#define LLC 12345678901234
#define LDC 12345678901234.0

double ld = LLC;
float  lf = LLC;

int almost( double v1, double v2 )
{
    return !( fabs( v1 - v2 ) < f );
}

typedef double DBL;
DBL r2_noneg( DBL x );

int main( void )
{
    double  lld = LLC;
    float   llf = LLC;

    --d;
    ++d;
    d/=2;
    if( almost( d, (DBL_MAX/2) ) ) fail(__LINE__);
    --e;
    ++e;
    e/=2;
    if( almost( e, -(DBL_MAX/2) ) ) fail(__LINE__);
    if( r2_noneg( 0 ) ) fail(__LINE__);
    if( ld != (double)LLC ) fail(__LINE__);
    if( lf != (float)LLC ) fail(__LINE__);
    if( ld != lld ) fail(__LINE__);
    if( lf != llf ) fail(__LINE__);
    if( LDC != ld ) fail(__LINE__);
    if( LDC != (double)LLC ) fail(__LINE__);
    if( (float)LDC != lf ) fail(__LINE__);
    if( (float)LDC != (float)LLC ) fail(__LINE__);

    _PASS;
}

/* Test for messed up handling of float constants in return statement;
 * see Bugzilla Bug 624. Must be located at end of source file to
 * provoke (or not) error.
 */

DBL r2_noneg( DBL x )
{
    if ( x <= 0.0 )
        return( 0.0 );
    else
        return( floor( 100 * x + .5000001 ) / 100 );
}
