#include "fail.h"

class S {
    int a;
    friend void foo( S * );
    friend int bar( S * );
};

static void foo( S *p )
{
    p->a = 1;
}

int bar( S *p )
{
    return ++(p->a);
}

int main()
{
    S x;

    foo( &x );
    if( bar( &x ) != 2 ) fail(__LINE__);
    _PASS
}
