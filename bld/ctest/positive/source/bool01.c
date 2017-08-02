#include "fail.h"
#include <stdbool.h>

/* See also init22.c test. */

/* Sanity check. */
char ac[ (int)3.999 == 3 ? 1 : -1 ];
char am[ ((int)3.999 == 3) * 2 - 1 ];

/* 0.5 converted to int is zero. */
char dc[ (int)0.5 == false ? 1 : -1 ];
char dm[ ((int)0.5 == false) * 2 - 1 ];

/* 0.5 converted to _Bool is non-zero/true. */
char dc[ (bool)0.5 == true ? 1 : -1 ];
char dt[ (bool)0.5 ? 1 : -1 ];
char dm[ ((bool)0.5 == true) * 2 - 1 ];

/* 0.0 is of course still zero/false. */
char fc[ (_Bool)0.0 == false ? 1 : -1 ];
char ft[ (_Bool)0.0 ? -1 : 1 ];
char fm[ ((_Bool)0.0 == false) * 2 - 1 ];

/* Non-zero integer converted to _Bool is true. */
char ic[ (_Bool)-16 == true ? 1 : -1 ];
char it[ (_Bool)-16 ? 1 : -1 ];
char im[ ((_Bool)-16 == true) * 2 - 1 ];

/* _Bool can generally be used wherever an int can be. */

int bool_add1( bool b, int i )
{
    return( b + i );
}

int bool_add2( bool b, int i )
{
    return( i + b );
}

int bool_add3( int i, bool b )
{
    return( b + i );
}

int bool_add4( int i, bool b )
{
    return( i + b );
}


int bool_sub1( bool b, int i )
{
    return( b - i );
}

int bool_sub2( bool b, int i )
{
    return( i - b );
}

int bool_sub3( int i, bool b )
{
    return( b - i );
}

int bool_sub4( int i, bool b )
{
    return( i - b );
}

int arr[] = { 1, 2, 3, 4, 5 };

/* Try using a _Bool as an array index. */
int idx( bool b )
{
    return( arr[b] );
}

int main( void )
{
    if( idx( 3 ) != 2 )  fail( __LINE__ );
    if( idx( -7 ) != 2 ) fail( __LINE__ );
    if( idx( .1 ) != 2 ) fail( __LINE__ );
    if( idx( 0 ) != 1 )  fail( __LINE__ );

    if( bool_add1( 2, 3 ) != 4 ) fail( __LINE__ );
    if( bool_add2( 3, 3 ) != 4 ) fail( __LINE__ );
    if( bool_add3( 2, 3 ) != 3 ) fail( __LINE__ );
    if( bool_add4( 3, 3 ) != 4 ) fail( __LINE__ );

    if( bool_sub1( 2, 3 ) != -2 ) fail( __LINE__ );
    if( bool_sub2( 3, 3 ) !=  2 ) fail( __LINE__ );
    if( bool_sub3( 2, 3 ) != -1 ) fail( __LINE__ );
    if( bool_sub4( 3, 3 ) !=  2 ) fail( __LINE__ );

    _PASS;
}
