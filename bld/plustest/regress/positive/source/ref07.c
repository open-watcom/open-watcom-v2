#include "fail.h"

struct D;
struct B {
    int b[10];
    D &foo( B &r )
    {
	return (D&) r;
    }
};
struct D : B {
    int d[10];
};

int main()
{
    D x;
    D &r = x.foo( x );

    if( &r != &x ) fail(__LINE__);
    _PASS;
}
