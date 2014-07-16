#include "fail.h"
#include <stdio.h>
#include <stdarg.h>


int __pascal f1( int i, int y, ... )
{
    va_list args;
    int v;

    va_start( args, y );
    while( i ) {
	v = va_arg( args, int );
	--i;
    }
    va_end( args );
    return( v );
}

int main()
{
    if( f1(1,1,1,2,3,4,5,6) != 1 ) fail(__LINE__);
    if( f1(2,1,1,2,3,4,5,6) != 2 ) fail(__LINE__);
    if( f1(3,1,1,2,3,4,5,6) != 3 ) fail(__LINE__);
    if( f1(4,1,1,2,3,4,5,6) != 4 ) fail(__LINE__);
    if( f1(5,1,1,2,3,4,5,6) != 5 ) fail(__LINE__);
    if( f1(6,1,1,2,3,4,5,6) != 6 ) fail(__LINE__);
    _PASS;
}
