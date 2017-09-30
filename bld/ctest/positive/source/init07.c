#include "fail.h"

int foo( int i )
{
    unsigned char auto1[] = {0,1,2,3,4,5,6,7,8,9};
    unsigned char auto0[10] = { 0 };
    return auto0[i]+auto1[i];
}

int main() {
    if( foo(0) != 0 ) _fail;
    if( foo(1) != 1 ) _fail;
    if( foo(2) != 2 ) _fail;
    if( foo(3) != 3 ) _fail;
    if( foo(4) != 4 ) _fail;
    if( foo(5) != 5 ) _fail;
    if( foo(6) != 6 ) _fail;
    if( foo(7) != 7 ) _fail;
    if( foo(8) != 8 ) _fail;
    if( foo(9) != 9 ) _fail;
    _PASS;
}
