#include "fail.h"
#include <stdio.h>

double foo( double *p, unsigned n )
{
    double max = p[n-1];
    int i;

    for( i = 0; i < n-1; ++i ) {
	max = max > p[i] ? max : p[i];
    }
    return( max );
}

int main()
{
    double a[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };

    double max = foo( a, sizeof(a) / sizeof(a[0]) );
    if( max < 4.9 || max > 5.1 ) fail(__LINE__);
    _PASS;
}
