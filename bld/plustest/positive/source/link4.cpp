#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include "link4.h"


S<int> x;
extern S<int> y;
S<int>::N z;
extern S<int>::N w;

int main()
{
    if( x.foo() != sizeof( int ) ) fail(__LINE__);
    if( x.foo() != y.foo() ) fail(__LINE__);
    if( z.foo() != 2 * sizeof( int ) ) fail(__LINE__);
    if( z.foo() != w.foo() ) fail(__LINE__);
    _PASS;
}
