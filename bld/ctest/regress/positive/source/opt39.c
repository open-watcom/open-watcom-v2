#include "fail.h"

int setzero( int a, int b )
{
    return a - b;
} 

int foo( a, b )
{
    int bar = a + b + 5;

    /* the bar test will fail in OW 1.0 and earlier since
       the non-flag setting of lea bar, [a+b+5] was optimized
       away */
    if (bar) {
        setzero ( a, b );
        return 2;
    }
    return 0;
}


int main( void )
{
    int bar = setzero( 5, 5 );
    if ( !foo( 5, 5 ) )
        _fail;
    _PASS;
}
