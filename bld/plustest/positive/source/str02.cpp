#include "fail.h"

long char *p;
char *q;

struct S {
    void foo()
    {
	p = L"012345678";
	q =  "abcdefghi";
    }
};

void foo( S *x )
{
    x->foo();
}

int main()
{
    S x;

    foo( &x );
    for( int i = 0; i < 9; ++i ) {
	if( q[i] != ( 'a' + i ) ) fail(__LINE__);
	if( p[i] != ( L'0' + i ) ) fail(__LINE__);
    }
    _PASS;
}
