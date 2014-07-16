#include "fail.h"

#ifndef __WATCOM_INT64__
#define __int64 long
#endif

struct S {
    double d;
    __int64 x;
};

void foo( S *p ) {
    p->d = 6.25;
    p->x = 0;
}

int main()
{
    S x;

    foo( &x );
    if( x.d != 6.25 ) _fail;
    if( x.x != 0 ) _fail;
    _PASS;
}
