#include "fail.h"
#include <stdio.h>
#include <malloc.h>
#include <float.h>

struct xx {
    int z;
    double x;
} e;

void dummy( struct xx **p )
{
    *p = &e;
}

int main()
{
    union {
	double d;
	struct xx *p;
    } uu;
    e.x = 1.0;
    dummy( &uu.p );
    e.x -= DBL_EPSILON;
    uu.d = uu.p->x;
    if( uu.d != e.x ) fail(__LINE__);
    _PASS;
}
