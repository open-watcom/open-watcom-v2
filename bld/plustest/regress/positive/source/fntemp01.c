#include "fail.h"
#include <stdio.h>

extern "C" {
    template <class T>
	T add( T x, T y )	// forced to C++ linkage
	{
	    return x + y;
	}
};

typedef char (*fc)( char, char );
typedef int (*fi)( int, int );
typedef void (*fv)( void );

fc rfc()
{
    return add;
}

fi rfi()
{
    return add;
}

int main()
{
    if( fv(rfc()) == fv(rfi()) ) fail(__LINE__);
    _PASS;
}
