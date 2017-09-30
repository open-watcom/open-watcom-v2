#include "fail.h"
#include <stdio.h>

double pi = 3.14159265358979323846264338328L;

double two_pi( void )
{
    return 2 * pi;
}

int main()
{
    double d;
    int i;

    d = two_pi();
    d *= 100;
    i = d;
    if( i < 627 || i > 629 ) fail(__LINE__);
    _PASS;
}
