#include "fail.h"
// trivial conversions during argument binding
#include <stdio.h>
#include <stdlib.h>

template <class T>
    struct OV {
	operator void *()
	{
	    return 0;
	}
    };

template <class T>
    int operator ==( OV<T> const &x, OV<T> const &y )
    {
	fail(__LINE__);
	return 0;
    }

template <class T>
    struct OK {
    };

template <class T>
    int operator ==( OK<T> const &x, OK<T> const &y )
    {
	return 0;
    }

template <class T>
    int foo( OV<T> &x, OV<T> &y )
    {
	return x == y;
    }

template <class T>
    int foo( OK<T> &x, OK<T> &y )
    {
	return x == y;
    }

int main()
{
    OV<int> d;
    OK<char> e;

    foo( d, d );
    foo( e, e );
    _PASS;
}
