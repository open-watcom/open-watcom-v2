#include "fail.h"

struct S {
    double d;
    __int64 x;
};

void foo( struct S *p ) {
    p->d = 6.25;
    p->x = 0;
}

int main()
{
    struct S x;

    foo( &x );
    if( x.d != 6.25 ) _fail;
    if( x.x != 0 ) _fail;
    _PASS;
}
