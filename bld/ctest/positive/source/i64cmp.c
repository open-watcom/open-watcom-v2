// 96/07/19 -- J.W.Welch    -- changed i64parts to long for 16-bit compiler

#include "fail.h"
#include <limits.h>

typedef struct {
    unsigned long       low;
    unsigned long       high;
} i64parts;

typedef __int64          signed_64;
typedef unsigned __int64 unsigned_64;

typedef union {
    signed_64           i64;
    unsigned_64         u64;
    i64parts            p;
} i64union;

int main()
{
    i64union    p1;
    i64union    p2;

    p1.p.low  = 0xffffffff;
    p1.p.high = 0xffffffff;
    p2.p.low  = 0xffffffff;
    p2.p.high = 0x7fffffff;

    // signed operations
    if( p1.i64 > p2.i64 ) fail( __LINE__ );
    if( p2.i64 < p1.i64 ) fail( __LINE__ );
    if( p1.i64 == p2.i64 ) fail( __LINE__ );
    if( p1.i64 != p1.i64 ) fail( __LINE__ );

    // unsigned operations
    if( p1.u64 < p2.u64 ) fail( __LINE__ );
    if( p2.u64 > p1.u64 ) fail( __LINE__ );
    if( p2.u64 == p1.u64 ) fail( __LINE__ );
    if( p2.u64 != p2.u64 ) fail( __LINE__ );

    _PASS;
}
