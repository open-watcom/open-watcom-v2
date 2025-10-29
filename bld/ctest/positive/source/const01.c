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

#ifdef _M_I86
#define DEF_INT_SIZE    2
#else
#define DEF_INT_SIZE    4
#endif

int main( int argc, char **argv )
{
/* C01 size=2/4 */
    if( sizeof( C01 ) != DEF_INT_SIZE ) fail(__LINE__);
/* C02 size=2/4 */
    if( sizeof( C02 ) != DEF_INT_SIZE ) fail(__LINE__);
/* C03 size=2/4 */
    if( sizeof( C03 ) != DEF_INT_SIZE ) fail(__LINE__);

/* C11 size=2/4 */
    if( sizeof( C11 ) != DEF_INT_SIZE ) fail(__LINE__);
/* C12 size=2/4 */
    if( sizeof( C12 ) != DEF_INT_SIZE ) fail(__LINE__);
/* C13 size=2/4 */
    if( sizeof( C13 ) != DEF_INT_SIZE ) fail(__LINE__);

/* C21 size=4/4 */
    if( sizeof( C21 ) != 4 ) fail(__LINE__);
/* C22 size=4/4 */
    if( sizeof( C22 ) != 4 ) fail(__LINE__);
/* C23 size=4/4 */
    if( sizeof( C23 ) != 4 ) fail(__LINE__);

/* C31 size=8/8 */
    if( sizeof( C31 ) != 8 ) fail(__LINE__);
/* C32 size=8/8 */
    if( sizeof( C32 ) != 8 ) fail(__LINE__);
/* C33 size=8/8 */
    if( sizeof( C33 ) != 8 ) fail(__LINE__);

/* I01 size=2/4 */
    if( sizeof( I01 ) != DEF_INT_SIZE ) fail(__LINE__);
/* I02 size=2/4 */
    if( sizeof( I02 ) != DEF_INT_SIZE ) fail(__LINE__);
/* I03 size=2/4 */
    if( sizeof( I03 ) != DEF_INT_SIZE ) fail(__LINE__);

/* I11 size=2/4 */
    if( sizeof( I11 ) != DEF_INT_SIZE ) fail(__LINE__);
/* I12 size=2/4 */
    if( sizeof( I12 ) != DEF_INT_SIZE ) fail(__LINE__);
/* I13 size=4/4 */
    if( sizeof( I13 ) != 4 ) fail(__LINE__);

/* I21 size=4/4 */
    if( sizeof( I21 ) != 4 ) fail(__LINE__);
/* I22 size=4/4 */
    if( sizeof( I22 ) != 4 ) fail(__LINE__);
/* I23 size=8/8 */
    if( sizeof( I23 ) != 8 ) fail(__LINE__);

/* I31 size=8/8 */
    if( sizeof( I31 ) != 8 ) fail(__LINE__);
/* I32 size=8/8 */
    if( sizeof( I32 ) != 8 ) fail(__LINE__);
/* I33 size=8/8 */
    if( sizeof( I33 ) != 8 ) fail(__LINE__);

/* E01 size=8/8 */
    if( sizeof( E01 ) != 8 ) fail(__LINE__);
/* E02 size=8/8 */
    if( sizeof( E02 ) != 8 ) fail(__LINE__);
/* E03 size=8/8 */
    if( sizeof( E03 ) != 8 ) fail(__LINE__);
/* E04 size=8/8 */
    if( sizeof( E04 ) != 8 ) fail(__LINE__);
/* E05 size=2/4 */
    if( sizeof( E05 ) != DEF_INT_SIZE ) fail(__LINE__);
/* E06 size=8/8 */
    if( sizeof( E06 ) != 8 ) fail(__LINE__);

    _PASS;
}
