/*
 * check for bugzilla bug #1164 comma operator not working when assigning structures
 */
#include "fail.h"

int main( void )
{
    struct S { int dummy; } a, b;

    a.dummy = 1;
    b.dummy = 2;
    a = (0, b);
    if( a.dummy != 2 ) fail(__LINE__);
    _PASS;
}
