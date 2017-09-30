#include "fail.h"
#include <stdio.h>

extern "C" {
    template <class T>
	T max( T x, T y )
	{
	    return ( x > y ) ? x : y;
	}
};

int q( int a, int b )
{
    return max(a,b);
}

char r( char a, char b )
{
    return max(a,b);
}

int main()
{
    if( q(1,2) != 2 ) fail(__LINE__);
    if( q('a','b') != 'b' ) fail(__LINE__);
    _PASS;
}
