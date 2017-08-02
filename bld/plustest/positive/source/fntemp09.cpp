#include "fail.h"

template <class T>
    void foo( int T::*m, T *p )
    {
	(p->*m) = 1;
    }

struct S {
    int a,b,c;
};

int main()
{
    S x;

    x.a = 1;
    x.b = 2;
    x.c = 3;
    foo( &S::b, &x );
    if( x.b != 1 ) fail(__LINE__);
    _PASS;
}
