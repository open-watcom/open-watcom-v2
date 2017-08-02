#include "fail.h"

#include <math.h>


double foo2 = 1e200 * 1e100;
#define FOO3 (1e200 * 1e100)
double foo4 = 1e300;

double testval = 60.0;

int main()
{
    if (testval >= 9.99999e299) fail(__LINE__);
    if (testval >= foo2) fail(__LINE__);
    if (testval >= FOO3) fail(__LINE__);
    if (testval >= foo4) fail(__LINE__);
    if (testval >= 1e300) fail(__LINE__);
    _PASS;
}
