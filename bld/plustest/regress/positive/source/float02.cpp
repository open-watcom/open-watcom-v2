#include "fail.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

void around( double d, double lo, double hi, unsigned line )
{
    if( d < lo || d > hi ) fail( line );
}

int main() {
    around( atan2( 5.0, 6.0 ), 0.694737, 0.694739, __LINE__ );
    around( atan2( DBL_MAX, DBL_MIN ), 1.570795, 1.570797, __LINE__ );
    around( atan2( DBL_MAX, -DBL_MIN ), 1.570795, 1.570797, __LINE__ );
    around( atan2( -DBL_MAX, DBL_MIN ), -1.570797, -1.570795, __LINE__ );
    around( atan2( -DBL_MAX, -DBL_MIN ), -1.570797, -1.570795, __LINE__ );
    around( atan2( DBL_MIN, DBL_MAX ), 0.000000, 0.000001, __LINE__ );
    around( atan2( -DBL_MIN, DBL_MAX ), -0.000001, 0.0000, __LINE__ );
    around( atan2( DBL_MIN, -DBL_MAX ), 3.141592, 3.141594, __LINE__ );
    around( atan2( -DBL_MIN, -DBL_MAX ), -3.141594, -3.141592, __LINE__ );
    _PASS;
}
