#include "fail.h"

struct A {
    const int i1;
    int i2;
};


int main()
{
    A a1 = { 1, 2 };
    const A a2 = { 3, 4 };

    if( a1.i1 != 1 ) fail( __LINE__ );
    if( a1.i2 != 2 ) fail( __LINE__ );
    if( a2.i1 != 3 ) fail( __LINE__ );
    if( a2.i2 != 4 ) fail( __LINE__ );

    _PASS;
}
