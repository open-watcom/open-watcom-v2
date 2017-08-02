#include "fail.h"

int f() {
    return __LINE__;
}

int *chk_p;
int (*chk_fp)();

int check_p( int *p, unsigned line )
{
    if( p != chk_p && p != NULL ) {
	fail(line);
	return( 1 );
    }
    return( 0 );
}

int check_fp( register int (*p)(), unsigned line )
{
    if( p != chk_fp && p != NULL ) {
	fail(line);
	return( 1 );
    }
    return( 0 );
}

int main() {
    int i;
    int d;
    int *p;
    int (*fp)();

    chk_p = &d;
    chk_fp = &f;
    for( i = 0; i < 16; ++i ) {
	p = ( i & 0x01 ) ? &d : 0;
	check_p( p, __LINE__ );
	p = ( i & 0x01 ) ? 0 : &d;
	check_p( p, __LINE__ );
	fp = ( i & 0x01 ) ? &f : 0;
	check_fp( fp, __LINE__ );
	fp = ( i & 0x01 ) ? 0 : &f;
	check_fp( fp, __LINE__ );
    }
    _PASS;
}
