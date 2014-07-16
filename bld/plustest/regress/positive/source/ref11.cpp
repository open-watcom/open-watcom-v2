#include "fail.h"
#include <float.h>

struct D {
    operator double &() { return d; }
    static double d;
};

double D::d = 1.0 - DBL_EPSILON;

struct S {
    operator double () { return x; }
    D x;
};

int main()
{
    S x;

    if( (DBL_EPSILON+(double)x) != 1 ) fail(__LINE__);
    _PASS;
}
