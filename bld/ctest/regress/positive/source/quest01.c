#include "fail.h"

int f( void ) {
    return __LINE__;
}

int *chk_p;
int (*chk_fp)( void );

int check_p( int *p, unsigned line )
{
    if( p != chk_p && p != NULL ) {
    fail(line);
    return( 1 );
    }
    return( 0 );
}

int check_fp( register int (*p)( void ), unsigned line )
{
    if( p != chk_fp && p != NULL ) {
    fail( line );
    return( 1 );
    }
    return( 0 );
}

#ifdef _M_IX86
char __far *foo( char *ptr )
{
    char _far *str;

    str = (ptr == NULL) ? (char __far *)NULL : ptr;
    return( str );
}

void check_farp( void )
{
    if( foo( 0 ) != NULL ) fail(__LINE__);
}
#else

void check_farp( void )
{
}
#endif

int main( void )
{
    int i;
    int d;
    int *p;
    int (*fp)( void );

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
    check_farp();
    _PASS;
}
