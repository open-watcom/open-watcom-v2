#include "fail.h"
// cannot add extra destination parm as last parm because ... parm lists
// have an unknown number of parms!
// fix: special case "..." functions
//      to have destination as first non-this parm (i.e., comes after 'this')
// schedule removal of special case in 10.0
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct si3 { long i1, i2; };

si3 nnprint(char *format, ...)
{
    va_list ap;
    si3 ii;
    
    va_start( ap, format );
    ii.i1 = (int) va_arg( ap, void * );
    ii.i2 = (int) va_arg( ap, void * );
    va_end( ap );
    return ii;
}

struct XX {
    si3 nnprint(char *format, ...);
};

si3 XX::nnprint(char *format, ...)
{
    va_list ap;
    si3 ii;
    
    va_start( ap, format );
    ii.i1 = (int) va_arg( ap, void * );
    ii.i2 = (int) va_arg( ap, void * );
    va_end( ap );
    return ii;
}

int main()
{
    {
    si3 d1;
    si3 d2;
    si3 xx;
    si3 yy;

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
