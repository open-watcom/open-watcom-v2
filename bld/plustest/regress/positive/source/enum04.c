#include "fail.h"
#include <stdio.h>

typedef enum {
    x = 1UI64 << 30,
    y = 1UI64 << 31,
#if defined(__386__)
    z = 1UI64 << 32,
#endif
} my_enum;

typedef enum {
    x2 = 1I64 << 29,
    z2 = 1I64 << 31,
#if defined(__386__)
    y2 = -1 << 15,
#endif
} my_enum2;

int main( int argc, char **argv )
{
    __int64	i = 1UI64 << 30;
    __int64	j = 1UI64 << 31;
    __int64	k = 1UI64 << 32;

    __int64	i2 = 1I64 << 29;
    __int64	j2 = -1 << 15;
    __int64	k2 = 1I64 << 31;

    if( i != x ) fail(__LINE__);
    if( j != y ) fail(__LINE__);
#if defined(__386__)
    if( k != z ) fail(__LINE__);
#endif

    if( i2 != x2 ) fail(__LINE__);
#if defined(__386__)
    if( j2 != y2 ) fail(__LINE__);
#endif
    if( k2 != z2 ) fail(__LINE__);
    _PASS;
}
