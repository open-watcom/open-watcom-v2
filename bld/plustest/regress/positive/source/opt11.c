#include "fail.h"
#include <math.h>
#include <stdio.h>
#include <new.h>

double *Magnitude( double *Length, double *v, int n )
{
    register int t;
    
    if(n<1) return 0;
    *Length=0.0;
    for(t=0; t<n; t++)
    *Length+=v[t]*v[t];
    Length = new (Length) double;	// try to force into EAX
    *Length=sqrt(*Length);
    return Length;
}


main()
{
    double x[2] = { 1.0, 2.0 };
    double mag;
    int n=2;
    
    Magnitude(&mag,x,n);
    // sqrt(5) ~= 2.23
    if( mag < 2.2 || mag > 2.3 ) fail(__LINE__);
    _PASS;
}
