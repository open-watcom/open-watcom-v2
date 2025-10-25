#include "fail.h"
#include <stdio.h>

#define C01  -0x7F
#define C02  0x7F
#define C03  0xFF

#define C11  -0x7FFF
#define C12  0x7FFF
#define C13  0xFFFF

#define C21  -0x7FFFFFFF
#define C22  0x7FFFFFFF
#define C23  0xFFFFFFFF

#define C31  -0x7FFFFFFFFFFFFFFF
#define C32  0x7FFFFFFFFFFFFFFF
#define C33  0xFFFFFFFFFFFFFFFF

#define I01  -127
#define I02  127
#define I03  255

#define I11  -32767
#define I12  32767
#define I13  65535

#define I21  -2147483647
#define I22  2147483647
#define I23  4294967295

#define I31  -9223372036854775807
#define I32  9223372036854775807
#define I33  18446744073709551615

#define E01  1UI64 << 30
#define E02  1UI64 << 31
#define E03  1UI64 << 32
#define E04  1I64 << 29
#define E05  -1 << 15
#define E06  1I64 << 31

int main( int argc, char **argv )
{
    if( sizeof( C01 ) != 4 ) fail(__LINE__);
    if( sizeof( C02 ) != 4 ) fail(__LINE__);
    if( sizeof( C03 ) != 4 ) fail(__LINE__);

    if( sizeof( C11 ) != 4 ) fail(__LINE__);
    if( sizeof( C12 ) != 4 ) fail(__LINE__);
    if( sizeof( C13 ) != 4 ) fail(__LINE__);

    if( sizeof( C21 ) != 4 ) fail(__LINE__);
    if( sizeof( C22 ) != 4 ) fail(__LINE__);
    if( sizeof( C23 ) != 4 ) fail(__LINE__);

    if( sizeof( C31 ) != 8 ) fail(__LINE__);
    if( sizeof( C32 ) != 8 ) fail(__LINE__);
    if( sizeof( C33 ) != 8 ) fail(__LINE__);

    if( sizeof( I01 ) != 4 ) fail(__LINE__);
    if( sizeof( I02 ) != 4 ) fail(__LINE__);
    if( sizeof( I03 ) != 4 ) fail(__LINE__);

    if( sizeof( I11 ) != 4 ) fail(__LINE__);
    if( sizeof( I12 ) != 4 ) fail(__LINE__);
    if( sizeof( I13 ) != 4 ) fail(__LINE__);

    if( sizeof( I21 ) != 4 ) fail(__LINE__);
    if( sizeof( I22 ) != 4 ) fail(__LINE__);
    if( sizeof( I23 ) != 8 ) fail(__LINE__);

    if( sizeof( I31 ) != 8 ) fail(__LINE__);
    if( sizeof( I32 ) != 8 ) fail(__LINE__);
    if( sizeof( I33 ) != 8 ) fail(__LINE__);

    if( sizeof( E01 ) != 8 ) fail(__LINE__);
    if( sizeof( E02 ) != 8 ) fail(__LINE__);
    if( sizeof( E03 ) != 8 ) fail(__LINE__);
    if( sizeof( E04 ) != 8 ) fail(__LINE__);
    if( sizeof( E05 ) != 4 ) fail(__LINE__);
    if( sizeof( E06 ) != 8 ) fail(__LINE__);

    _PASS;
}
