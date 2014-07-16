#include "fail.h"

struct fields {
    unsigned x1:1;
    unsigned x2:2;
    unsigned x3:3;
};

void bar( int, unsigned );

void foo( fields *p, fields &r )
{
    bar( (p->x3, r.x2), __LINE__ );
    bar( (r.x2,p->x1+1), __LINE__ );
    bar( r.x2, __LINE__ );
    bar( p->x3 - 1, __LINE__ );
}

void bar( int v, unsigned line )
{
    if( v != 2 ) fail(line);
}

int main()
{
    fields x;

    x.x1 = 1;
    x.x2 = 2;
    x.x3 = 3;
    foo( &x, x );
    _PASS;
}
