#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

template<class T>
    inline T foo(T value)
    {
	return value + value * 3;
    }

struct Bar {
    int field;
};

int f1(Bar bar)
{
    return foo(bar.field);
}

int f2(Bar __far &bar)
{
    return foo(bar.field);
}

int main()
{
    Bar dummy;

    dummy.field = 1;
    dummy.field = f1( dummy );
    dummy.field = f2( dummy );
    if( dummy.field != 16 ) fail(__LINE__);
    _PASS;
}
