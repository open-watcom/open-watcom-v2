#include "fail.h"
#include <stdio.h>
#include <limits.h>
#include <float.h>

/* Test const folding for out of range comparisons to make sure the results
 * are correct. Also make sure floating-point comparisons are not incorrectly
 * folded (was broken in 1.6).
 */

int cmp_U1_lo_lt( unsigned char x )
{
    if( x < 0 ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U1_lo_ge( unsigned char x )
{
    if( x >= 0 ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U1_hi_le( unsigned char x )
{
    if( x <= UCHAR_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U1_hi_gt( unsigned char x )
{
    if( x > UCHAR_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I1_lo_lt( signed char x )
{
    if( x < SCHAR_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I1_lo_ge( signed char x )
{
    if( x >= SCHAR_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I1_hi_le( signed char x )
{
    if( x <= SCHAR_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I1_hi_gt( signed char x )
{
    if( x > SCHAR_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U2_lo_lt( unsigned short x )
{
    if( x < 0 ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U2_lo_ge( unsigned short x )
{
    if( x >= 0 ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U2_hi_le( unsigned short x )
{
    if( x <= USHRT_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U2_hi_gt( unsigned short x )
{
    if( x > USHRT_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I2_lo_lt( signed short x )
{
    if( x < SHRT_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I2_lo_ge( signed short x )
{
    if( x >= SHRT_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I2_hi_le( signed short x )
{
    if( x <= SHRT_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I2_hi_gt( signed short x )
{
    if( x > SHRT_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U4_lo_lt( unsigned long x )
{
    if( x < 0 ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U4_lo_ge( unsigned long x )
{
    if( x >= 0 ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U4_hi_le( unsigned long x )
{
    if( x <= ULONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U4_hi_gt( unsigned long x )
{
    if( x > ULONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I4_lo_lt( signed long x )
{
    if( x < LONG_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I4_lo_ge( signed long x )
{
    if( x >= LONG_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I4_hi_le( signed long x )
{
    if( x <= LONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I4_hi_gt( signed long x )
{
    if( x > LONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U8_lo_lt( unsigned long long x )
{
    if( x < 0 ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_U8_lo_ge( unsigned long long x )
{
    if( x >= 0 ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U8_hi_le( unsigned long long x )
{
    if( x <= ULLONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_U8_hi_gt( unsigned long long x )
{
    if( x > ULLONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I8_lo_lt( signed long long x )
{
    if( x < LLONG_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_I8_lo_ge( signed long long x )
{
    if( x >= LLONG_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I8_hi_le( signed long long x )
{
    if( x <= LLONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_I8_hi_gt( signed long long x )
{
    if( x > LLONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}
int cmp_FS_lo_lt( float x )
{
    if( x < LONG_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_FS_lo_ge( float x )
{
    if( x >= LONG_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_FS_hi_le( float x )
{
    if( x <= LONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_FS_hi_gt( float x )
{
    if( x > LONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_FD_lo_lt( double x )
{
    if( x < LONG_MIN ) {
        return( 1 );
    }
    return( 0 );
}

int cmp_FD_lo_ge( double x )
{
    if( x >= LONG_MIN ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_FD_hi_le( double x )
{
    if( x <= LONG_MAX ) {
        return( 0 );
    }
    return( 1 );
}

int cmp_FD_hi_gt( double x )
{
    if( x > LONG_MAX ) {
        return( 1 );
    }
    return( 0 );
}

unsigned char       uc;
signed char         sc;
unsigned short      us;
signed short        ss;
unsigned long       ul;
signed long         sl;
float               fsx = FLT_MAX;
float               fsn = -FLT_MAX;
double              fdx = DBL_MAX;
double              fdn = -DBL_MAX;

int main( void )
{
    if( cmp_U1_lo_lt( uc ) ) fail( __LINE__ );
    if( cmp_U1_lo_ge( uc ) ) fail( __LINE__ );
    if( cmp_U1_hi_le( uc ) ) fail( __LINE__ );
    if( cmp_U1_hi_gt( uc ) ) fail( __LINE__ );
    if( cmp_I1_lo_lt( sc ) ) fail( __LINE__ );
    if( cmp_I1_lo_ge( sc ) ) fail( __LINE__ );
    if( cmp_I1_hi_le( sc ) ) fail( __LINE__ );
    if( cmp_I1_hi_gt( sc ) ) fail( __LINE__ );
    if( cmp_U2_lo_lt( us ) ) fail( __LINE__ );
    if( cmp_U2_lo_ge( us ) ) fail( __LINE__ );
    if( cmp_U2_hi_le( us ) ) fail( __LINE__ );
    if( cmp_U2_hi_gt( us ) ) fail( __LINE__ );
    if( cmp_I2_lo_lt( ss ) ) fail( __LINE__ );
    if( cmp_I2_lo_ge( ss ) ) fail( __LINE__ );
    if( cmp_I2_hi_le( ss ) ) fail( __LINE__ );
    if( cmp_I2_hi_gt( ss ) ) fail( __LINE__ );
    if( cmp_U4_lo_lt( ul ) ) fail( __LINE__ );
    if( cmp_U4_lo_ge( ul ) ) fail( __LINE__ );
    if( cmp_U4_hi_le( ul ) ) fail( __LINE__ );
    if( cmp_U4_hi_gt( ul ) ) fail( __LINE__ );
    if( cmp_I4_lo_lt( sl ) ) fail( __LINE__ );
    if( cmp_I4_lo_ge( sl ) ) fail( __LINE__ );
    if( cmp_I4_hi_le( sl ) ) fail( __LINE__ );
    if( cmp_I4_hi_gt( sl ) ) fail( __LINE__ );
    if( cmp_U8_lo_lt( ul ) ) fail( __LINE__ );
    if( cmp_U8_lo_ge( ul ) ) fail( __LINE__ );
    if( cmp_U8_hi_le( ul ) ) fail( __LINE__ );
    if( cmp_U8_hi_gt( ul ) ) fail( __LINE__ );
    if( cmp_I8_lo_lt( sl ) ) fail( __LINE__ );
    if( cmp_I8_lo_ge( sl ) ) fail( __LINE__ );
    if( cmp_I8_hi_le( sl ) ) fail( __LINE__ );
    if( cmp_I8_hi_gt( sl ) ) fail( __LINE__ );

    if( !cmp_FS_lo_lt( fsn ) ) fail( __LINE__ );
    if( !cmp_FS_lo_ge( fsn ) ) fail( __LINE__ );
    if( !cmp_FS_hi_le( fsx ) ) fail( __LINE__ );
    if( !cmp_FS_hi_gt( fsx ) ) fail( __LINE__ );
    if( !cmp_FD_lo_lt( fdn ) ) fail( __LINE__ );
    if( !cmp_FD_lo_ge( fdn ) ) fail( __LINE__ );
    if( !cmp_FD_hi_le( fdx ) ) fail( __LINE__ );
    if( !cmp_FD_hi_gt( fdx ) ) fail( __LINE__ );

    _PASS;
}
