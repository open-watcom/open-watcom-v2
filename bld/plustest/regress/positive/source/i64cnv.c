// 96/07/19 -- J.W.Welch        -- changed i64parts to long for i86 compiler
//                              -- changed i4, u4 to long for i86 compiler

#include "fail.h"
#include <limits.h>

#ifdef __WATCOM_INT64__

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

signed char     i1 = 0x80;
unsigned char   u1 = 0xff;
signed short    i2 = 0x8000;
unsigned short  u2 = 0xffff;
signed long     i4 = 0x80000000;
unsigned long   u4 = 0xffffffff;

int main()
{
    i64union    value;

    value.i64 = i1;
    if( value.p.low != 0xffffff80 || value.p.high != 0xffffffff ) fail( __LINE__ );
    value.i64 = i2;
    if( value.p.low != 0xffff8000 || value.p.high != 0xffffffff ) fail( __LINE__ );
    value.i64 = i4;
    if( value.p.low != 0x80000000 || value.p.high != 0xffffffff ) fail( __LINE__ );
    value.i64 = u1;
    if( value.p.low != 0x000000ff || value.p.high != 0 ) fail( __LINE__ );
    value.i64 = u2;
    if( value.p.low != 0x0000ffff || value.p.high != 0 ) fail( __LINE__ );
    value.i64 = u4;
    if( value.p.low != 0xffffffff || value.p.high != 0 ) fail( __LINE__ );
    _PASS;
}

#else

ALWAYS_PASS

#endif
