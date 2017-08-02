#include "fail.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

struct S {
    int i,j;
};

int weird_ellipse_fn( int control, ... )
{
    int val;
    va_list args;
    int S::* mp;
    static S *p;

    val = 0;
    va_start( args, control );
    if( control != 0 ) {
	val += p->*va_arg( args, int S::* );
	mp = va_arg( args, int S::* );
	val += p->*mp;
    } else {
	p = va_arg( args, S * );
    }
    va_end( args );
    return( val );
}

S dummy = { -10, 10 };

int main()
{
    if( weird_ellipse_fn( 0, &dummy ) != 0 ) {
	fail(__LINE__);
    }
    if( weird_ellipse_fn( 1, &S::j, &S::j ) != 20 ) {
	fail(__LINE__);
    }
    if( weird_ellipse_fn( 1, &S::i, &S::i ) != -20 ) {
	fail(__LINE__);
    }
    if( weird_ellipse_fn( 1, &S::i, &S::j ) != 0 ) {
	fail(__LINE__);
    }
    if( weird_ellipse_fn( 1, &S::j, &S::i ) != 0 ) {
	fail(__LINE__);
    }
    _PASS;
}
