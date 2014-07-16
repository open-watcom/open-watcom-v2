#include "fail.h"

struct S {
    int a;
    S() : a(0) {}
    void operator() (int x)
    {
	a += x;
    }
    void operator() (int x, int y)
    {
	a += x + y;
    }
    void operator() (int x, int y,int z)
    {
	a += x + y + z;
    }
};

int main()
{
    S x;

    x( 1 );
    x( 2, 3 );
    x( 4, 5, 6 );
    if( x.a != (6*7/2) ) fail(__LINE__);
    _PASS;
}
