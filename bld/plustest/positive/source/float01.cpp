#include "fail.h"
#include <float.h>
#include <math.h>

double d = DBL_MAX;
double e = -DBL_MAX;
double f = DBL_MAX*DBL_EPSILON;

int almost( double v1, double v2 )
{
    return !( fabs( v1 - v2 ) < f );
}

int main() {
    --d;
    ++d;
    d/=2;
    if( almost( d, (DBL_MAX/2) ) ) fail(__LINE__);
    --e;
    ++e;
    e/=2;
    if( almost( e, -(DBL_MAX/2) ) ) fail(__LINE__);
    _PASS;
}
