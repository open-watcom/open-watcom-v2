#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

template <class T1>
    struct Bar {
	T1 a;
	Bar(T1 x) : a(x) {}
    };

template<class T2>
    void foo(T2 a)
    {
	Bar<T2> *p = new Bar<T2>(a);
	if( p->a != 1 ) fail(__LINE__);
    }

int main()
{
    foo(1);
    _PASS;
}
