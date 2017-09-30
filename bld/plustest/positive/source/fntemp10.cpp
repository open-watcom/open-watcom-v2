#include "fail.h"
#include <stdarg.h>
#include <string.h>

#if __WATCOMC__ > 1060

template <class T>
    void foo( T *q, int x, int y, ... )
    {
	int v;
	int *p;
	va_list args;

	if( x != 1 ) fail(__LINE__);
	if( y != 2 ) fail(__LINE__);
	p = &q->i;
	va_start( args, y );
	for(;;) {
	    v = va_arg( args, int );
	    if( v == -1 ) break;
	    if( v != *p ) fail(__LINE__);
	    ++p;
	}
	va_end( args );
    }

struct S {int i,j,k;};
struct Q {int a,b,c,d,e,f,i,j,k;};

int main() {
    S x;
    x.i = 1;
    x.j = 2;
    x.k = 3;
    Q y;
    memset( &y, -1, sizeof( y ) );
    y.i = 11;
    y.j = 12;
    y.k = 13;
    foo( &x, 1, 2, 1, 2, 3, -1 );
    foo( &y, 1, 2, 11, 12, 13, -1 );
    _PASS;
}
#else

int main()
{
    _PASS;
}

#endif
