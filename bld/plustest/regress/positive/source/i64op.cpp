// 96/07/19 -- J.W.Welch    -- converted to long to make i86 compiler work

#include "fail.h"
#include <limits.h>

#ifdef __WATCOM_INT64__

typedef unsigned long part;

typedef struct {
    part low;
    part high;
} i64parts;

typedef __int64          signed_64;
typedef unsigned __int64 unsigned_64;

typedef union {
    signed_64           i64;
    unsigned_64         u64;
    i64parts            p;
} i64union;

signed_64 add( unsigned_64 p1, unsigned_64 p2 )
{
    return( p1 + p2 );
}

signed_64 sub( signed_64 p1, signed_64 p2 )
{
    return( p1 - p2 );
}

signed_64 mul( signed_64 p1, signed_64 p2 )
{
    return( p1 * p2 );
}

signed_64 my_div( signed_64 p1, signed_64 p2 )
{
    return( p1 / p2 );
}

unsigned_64 my_or( unsigned_64 p1, unsigned_64 p2 )
{
    return( p1 | p2 );
}

unsigned_64 my_and( unsigned_64 p1, unsigned_64 p2 )
{
    return( p1 & p2 );
}

unsigned_64 my_xor( unsigned_64 p1, unsigned_64 p2 )
{
    return( p1 ^ p2 );
}

unsigned_64 shl( unsigned_64 p1, unsigned p2 )
{
    return( p1 << p2 );
}


unsigned_64 shr( unsigned_64 p1, unsigned p2 )
{
    return( p1 >> p2 );
}

int main()
{
    i64union    result;
    i64union    p1;
    i64union    p2;


    result.i64 = add( ULONG_MAX, 0x2 );
    if( result.p.low != 1 || result.p.high != 1 ) fail( __LINE__ );
    result.i64 = sub( 0, 1 );
    if( result.p.low != ULONG_MAX || result.p.high != ULONG_MAX ) fail( __LINE__ );
    result.i64 = mul( 0x10000000, 0x10 );
    if( result.p.low != 0 || result.p.high != 1 ) fail( __LINE__ );
    result.i64 = my_div( result.i64, 0x10 );
    if( result.p.low != 0x10000000 || result.p.high != 0 ) fail( __LINE__ );

    #define PATTERN_1   0x10101010
    #define PATTERN_2   0x71717171

    p1.p.low = PATTERN_1; p1.p.high = PATTERN_1;
    p2.p.low = PATTERN_2; p2.p.high = PATTERN_2;
    result.u64 = my_or( p1.u64, p2.u64 );
    if( result.p.low != (PATTERN_1|PATTERN_2) || result.p.high != (PATTERN_1|PATTERN_2) ) fail( __LINE__ );
    result.u64 = my_and( result.u64, p2.u64 );
    if( result.p.low != p2.p.low || result.p.high != p2.p.high ) fail( __LINE__ );
    result.u64 = my_xor( p1.u64, p2.u64 );
    if( result.p.low != (PATTERN_1^PATTERN_2) || result.p.high != (PATTERN_1^PATTERN_2) ) fail( __LINE__ );

    result.u64 = shl( p1.u64, 4 );
    if( result.p.low != (part)(PATTERN_1 << 4)
        || result.p.high != (part)((PATTERN_1<<4)|((PATTERN_1&0xf0000000)>>28))) fail( __LINE__ );
    result.u64 = shl( p1.u64, 36 );
    if( result.p.low != 0 || result.p.high != (part)(PATTERN_1<<4)) fail( __LINE__ );
    result.u64 = shr( p1.u64, 8 );
    if( result.p.low != PATTERN_1 || result.p.high != (part)(PATTERN_1>>8) ) fail( __LINE__ );
    result.u64 = shr( p1.u64, 40 );
    if( result.p.low != (part)(PATTERN_1>>8) || result.p.high != 0 ) fail( __LINE__ );

    _PASS;
}

#else

ALWAYS_PASS

#endif
