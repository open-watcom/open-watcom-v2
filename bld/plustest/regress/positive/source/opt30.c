#include "fail.h"
#include <limits.h>

#define LIMX	(8)
#define LIM	(1<<(LIMX))
#define INC	( 1 << (( sizeof( unsigned ) * CHAR_BIT ) - LIMX ) )

#if __WATCOM_REVISION__>7
static unsigned long c, d;

static void f( unsigned long *ctr, unsigned a )
{
    a = a;
    (*ctr)++;
}

int main()
{
    unsigned limit = LIM;
    unsigned addr;
    unsigned addr_m;

    if( limit != 0 ) {
	addr_m = 0;
	while( addr_m < LIM ) {
	    addr = addr_m * INC;
	    f( &c, addr );
	    if( c == 0 )	// will never be true
		break;
	    addr_m++;
	}
	if( c != LIM ) fail(__LINE__);
	addr_m = 0;
	while( addr_m < LIM ) {
	    addr = addr_m * INC;
	    f( &d, addr );
	    addr_m++;
	}
	if( d != LIM ) fail(__LINE__);
    } else {
	fail(__LINE__);
    }
    _PASS;
}
#else
ALWAYS_PASS
#endif
