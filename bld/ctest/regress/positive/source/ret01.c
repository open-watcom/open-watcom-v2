#include "fail.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct si3 { long i1, i2; };

struct si3 nnprint(char *format, ...)
{
    va_list ap;
    struct si3 ii;

    va_start( ap, format );
    ii.i1 = (int) va_arg( ap, void * );
    ii.i2 = (int) va_arg( ap, void * );
    va_end( ap );
    return ii;
}

int main()
{
    {
    struct si3 d1;
    struct si3 d2;
    struct si3 xx;
    struct si3 yy;

    d1.i1 = 1;
    d1.i2 = 2;
    d2.i1 = 3;
    d2.i2 = 4;
    xx = nnprint("..", &d1, &d2);
    if( d1.i1 != 1 || d1.i2 != 2 || d2.i1 != 3 || d2.i2 != 4 ) {
        fail(__LINE__);
    }
    if( xx.i1 != (int) &d1 || xx.i2 != (int) &d2 ) {
        fail(__LINE__);
    }
    d1.i1 = 1;
    d1.i2 = 2;
    d2.i1 = 3;
    d2.i2 = 4;
    yy = nnprint("..", &d1, &d2);
    if( d1.i1 != 1 || d1.i2 != 2 || d2.i1 != 3 || d2.i2 != 4 ) {
        fail(__LINE__);
    }
    if( yy.i1 != (int) &d1 || yy.i2 != (int) &d2 ) {
        fail(__LINE__);
    }
    }
    _PASS;
}
