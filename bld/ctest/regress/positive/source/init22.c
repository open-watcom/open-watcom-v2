#include "fail.h"
#include <stdlib.h>
#include <string.h>

int fni( int a );

/* Test initialization of _Bool objects */

// any non-zero scalar gets converted to 1
_Bool ab[] = {
    32,
    0x20,
    3.1415,
    2.2f,
    (void *)4,
    ab,
    &ab,
    (void (*)()) 4,
    fni,
    0x2000000000,
};

// any zero scalar gets converted to 0:
_Bool cd[] = {
    0,
    0x0,
    0.000,
    0.0f,
    (void *)0,
    (void (*)()) 0,
    0x0000ULL,
    0LL
};

// Expressions using _Bool are compile-time constants:
char ef[(_Bool) -0.5 == 1 ? 1 : -1];
char gh[(_Bool) -0.0 == 0 ? 1 : -1];

// _Bool works as a struct element and bitfield type:
struct ij {
    _Bool i: 1;
    _Bool j;
} ij;

int fni( int a )
{
    _Bool   b = a;

    return( b );
}

int fnd( double a )
{
    _Bool   b = a;

    return( b );
}

int fnp( void *a )
{
    _Bool   b = a;

    return( b );
}

int main( void )
{
    char    *b;

    // note - ISO standard doesn't guarantee that sizeof( _Bool ) is 1
    b = (char *)ab;
    for( int i = 0; i < sizeof( ab ) / sizeof( ab[0] ); ++i ) {
        if( b[i] != 1 ) fail( __LINE__ );
    }

    if( fni( 42 ) + fnd( 6.28 ) + fnp( &main ) != 3 ) fail( __LINE__ );
    _PASS;
}
