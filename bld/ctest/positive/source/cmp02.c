#include "fail.h"
#include <stdio.h>
#include <limits.h>

/* Test const folding for comparisons against constants which are completely
 * outside the variable's type range. Was broken in 1.6.
 */

int cmp_U1_hi_eq( unsigned char x )
{
    if( x == UCHAR_MAX + 1 ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U2_hi_eq( unsigned short x )
{
    if( x == USHRT_MAX + 1L ) {
        return( 1 );
    }
    return( 0 );
}

#ifdef __WATCOM_INT64__
int cmp_U4_hi_eq( unsigned long x )
{
    if( x == ULONG_MAX + 1LL ) {
        return( 1 );
    }
    return( 0 );
}
#endif

/* Test comparisons of signed and/or unsigned chars (not shorts, ints, etc.).
 * Watcom 11.0 and later (up to 1.9) incorrectly generated the comparisons
 * as unsigned, which failed when comparing negative signed ints.
 */

int cmp_sc_sc_l( signed char c1, signed char c2 )
{
    return( c1 < c2 );
}

int cmp_sc_sc_g( signed char c1, signed char c2 )
{
    return( c1 > c2 );
}

int cmp_uc_uc_l( unsigned char c1, unsigned char c2 )
{
    return( c1 < c2 );
}

int cmp_uc_uc_g( unsigned char c1, unsigned char c2 )
{
    return( c1 > c2 );
}

int cmp_uc_sc_l( unsigned char c1, signed char c2 )
{
    return( c1 < c2 );
}

int cmp_sc_uc_g( signed char c1, unsigned char c2 )
{
    return( c1 > c2 );
}

#if __STDC_VERSION__ >= 199901L
/* The _Bool type is much like unsigned char */

int cmp_bl_sc_l( _Bool c1, signed char c2 )
{
    return( c1 < c2 );
}

int cmp_uc_bl_l( unsigned c1, _Bool c2 )
{
    return( c1 < c2 );
}
#endif

unsigned char       uc;
unsigned short      us;
unsigned long       ul;

int main( void )
{
    if( cmp_U1_hi_eq( uc ) ) fail( __LINE__ );
    if( cmp_U2_hi_eq( us ) ) fail( __LINE__ );
#ifdef __WATCOM_INT64__
    if( cmp_U4_hi_eq( ul ) ) fail( __LINE__ );
#endif

    if( cmp_sc_sc_l( 1, -1 ) ) fail( __LINE__ );
    if( cmp_sc_sc_l( 127, -127 ) ) fail( __LINE__ );
    if( cmp_sc_sc_l( 1, -127 ) ) fail( __LINE__ );
    if( cmp_uc_uc_l( 128, 1 ) ) fail( __LINE__ );
    if( cmp_uc_uc_l( 255, 128 ) ) fail( __LINE__ );
    if( cmp_uc_uc_l( 255, 1 ) ) fail( __LINE__ );
    if( cmp_sc_sc_g( -1, 1 ) ) fail( __LINE__ );
    if( cmp_sc_sc_g( -127, 127 ) ) fail( __LINE__ );
    if( cmp_sc_sc_g( -127, 1 ) ) fail( __LINE__ );
    if( cmp_uc_uc_g( 1, 128 ) ) fail( __LINE__ );
    if( cmp_uc_uc_g( 128, 255 ) ) fail( __LINE__ );
    if( cmp_uc_uc_g( 1, 255 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 1, -1 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 127, -127 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 128, -127 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 255, -127 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 255, 1 ) ) fail( __LINE__ );
    if( cmp_uc_sc_l( 1, -127 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( -1, 1 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( -127, 127 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( -127, 128 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( -127, 255 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( 1, 255 ) ) fail( __LINE__ );
    if( cmp_sc_uc_g( -127, 1 ) ) fail( __LINE__ );

#if __STDC_VERSION__ >= 199901L
    if( cmp_bl_sc_l( 1, -1 ) ) fail( __LINE__ );
    if( cmp_uc_bl_l( 128, 1 ) ) fail( __LINE__ );
#endif

    _PASS;
}
