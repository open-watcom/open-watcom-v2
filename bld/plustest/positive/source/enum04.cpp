#include "fail.h"
#include <stdio.h>

#define TEST_SIGNED(x) ( ( x - 1 ) - x < 0 )
#define TEST_UNSIGNED(x) ( ( x - 1 ) - x > 0 )

typedef enum {
    en81_3 = -0x7F,
    en81_1 = 0x7F,
    en81_2 = -0x7F
} enum_s81;

typedef enum {
    en82_3 = 0x7F,
    en82_1 = 0xFF,
    en82_2 = 0x7F
} enum_u82;

typedef enum {
    en161_3 = -0x7F,
    en161_1 = 0xFF,
    en161_2 = -0x7F
} enum_s161;

typedef enum {
    en162_3 = -0x7FFF,
    en162_1 = 0x7FFF,
    en162_2 = -0x7FFF
} enum_s162;

typedef enum {
    en163_3 = 0x7FFF,
    en163_1 = 0xFFFF,
    en163_2 = 0x7FFF
} enum_u163;

typedef enum {
    en321_3 = -0x7FFF,
    en321_1 = 0xFFFF,
    en321_2 = -0x7FFF
} enum_s321;

typedef enum {
    en322_3 = -0x7FFFFFFF,
    en322_1 = 0x7FFFFFFF,
    en322_2 = -0x7FFFFFFF
} enum_s322;

typedef enum {
    en323_3 = 0x7FFFFFFF,
    en323_1 = 0xFFFFFFFF,
    en323_2 = 0x7FFFFFFF
} enum_u323;

typedef enum {
    en641_3 = -0x7FFFFFFF,
    en641_1 = 0xFFFFFFFF,
    en641_2 = -0x7FFFFFFF
} enum_s641;

typedef enum {
    en642_3 = -0x7FFFFFFFFFFFFFFF,
    en642_1 = 0x7FFFFFFFFFFFFFFF,
    en642_2 = -0x7FFFFFFFFFFFFFFF
} enum_s642;

typedef enum {
    en643_3 = 0x7FFFFFFFFFFFFFFF,
    en643_1 = 0xFFFFFFFFFFFFFFFF,
    en643_2 = 0x7FFFFFFFFFFFFFFF
} enum_u643;

typedef enum {
    en1_81_3 = 0x7F,
    en1_81_1 = -0x7F,
    en1_81_2 = 0x7F
} enum1_s81;

typedef enum {
    en1_82_3 = 0xFF,
    en1_82_1 = 0x7F,
    en1_82_2 = 0xFF
} enum1_u82;

typedef enum {
    en1_161_3 = 0xFF,
    en1_161_1 = -0x7F,
    en1_161_2 = 0xFF
} enum1_s161;

typedef enum {
    en1_162_3 = 0x7FFF,
    en1_162_1 = -0x7FFF,
    en1_162_2 = 0x7FFF
} enum1_s162;

typedef enum {
    en1_163_3 = 0xFFFF,
    en1_163_1 = 0x7FFF,
    en1_163_2 = 0xFFFF
} enum1_u163;

typedef enum {
    en1_321_3 = 0xFFFF,
    en1_321_1 = -0x7FFF,
    en1_321_2 = 0xFFFF
} enum1_s321;

typedef enum {
    en1_322_3 = 0x7FFFFFFF,
    en1_322_1 = -0x7FFFFFFF,
    en1_322_2 = 0x7FFFFFFF
} enum1_s322;

typedef enum {
    en1_323_3 = 0xFFFFFFFF,
    en1_323_1 = 0x7FFFFFFF,
    en1_323_2 = 0xFFFFFFFF
} enum1_u323;

typedef enum {
    en1_641_3 = 0xFFFFFFFF,
    en1_641_1 = -0x7FFFFFFF,
    en1_641_2 = 0xFFFFFFFF
} enum1_s641;

typedef enum {
    en1_642_3 = 0x7FFFFFFFFFFFFFFF,
    en1_642_1 = -0x7FFFFFFFFFFFFFFF,
    en1_642_2 = 0x7FFFFFFFFFFFFFFF
} enum1_s642;

typedef enum {
    en1_643_3 = 0xFFFFFFFFFFFFFFFF,
    en1_643_1 = 0x7FFFFFFFFFFFFFFF,
    en1_643_2 = 0xFFFFFFFFFFFFFFFF
} enum1_u643;

typedef enum {
    x = 1UI64 << 30,
    y = 1UI64 << 31,
    z = 1UI64 << 32,
} my_enum;

typedef enum {
    x2 = 1I64 << 29,
    y2 = -1 << 15,
    z2 = 1I64 << 31,
} my_enum2;

int main( int argc, char **argv )
{
    __int64	i = 1UI64 << 30;
    __int64	j = 1UI64 << 31;
    __int64	k = 1UI64 << 32;

    __int64	i2 = 1I64 << 29;
    __int64	j2 = -1 << 15;
    __int64	k2 = 1I64 << 31;

#ifdef __SW_EI
    if( sizeof( enum_s81 ) != sizeof(int) ) fail(__LINE__);
    if( sizeof( enum_u82 ) != sizeof(int) ) fail(__LINE__);
    if( sizeof( enum_s161 ) != sizeof(int) ) fail(__LINE__);
    if( sizeof( enum_s162 ) != sizeof(int) ) fail(__LINE__);
    if( sizeof( enum_u163 ) != sizeof(int) ) fail(__LINE__);
#else
    if( sizeof( enum_s81 ) != sizeof(char) ) fail(__LINE__);
    if( sizeof( enum_u82 ) != sizeof(char) ) fail(__LINE__);
    if( sizeof( enum_s161 ) != sizeof(short) ) fail(__LINE__);
    if( sizeof( enum_s162 ) != sizeof(short) ) fail(__LINE__);
    if( sizeof( enum_u163 ) != sizeof(short) ) fail(__LINE__);
#endif
    if( sizeof( enum_s321 ) != 4 ) fail(__LINE__);
    if( sizeof( enum_s322 ) != 4 ) fail(__LINE__);
    if( sizeof( enum_u323 ) != 4 ) fail(__LINE__);
    if( sizeof( enum_s641 ) != 8 ) fail(__LINE__);
    if( sizeof( enum_s642 ) != 8 ) fail(__LINE__);
    if( sizeof( enum_u643 ) != 8 ) fail(__LINE__);
    if( i != x ) fail(__LINE__);
    if( j != y ) fail(__LINE__);
    if( k != z ) fail(__LINE__);

    if( i2 != x2 ) fail(__LINE__);
    if( j2 != y2 ) fail(__LINE__);
    if( k2 != z2 ) fail(__LINE__);

// test signed values

    if( TEST_UNSIGNED( en81_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en82_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en161_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en162_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en321_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en322_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en641_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en642_1 ) ) fail(__LINE__);
#if !defined( __I86__ )
    if( TEST_UNSIGNED( en163_1 ) ) fail(__LINE__);
#endif

// test unsigned values

#if defined( __I86__ )
    if( TEST_SIGNED( en163_1 ) ) fail(__LINE__);
#endif
    if( TEST_SIGNED( en323_1 ) ) fail(__LINE__);
    if( TEST_SIGNED( en643_1 ) ) fail(__LINE__);

// test signed values

    if( TEST_UNSIGNED( en1_81_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_82_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_161_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_162_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_321_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_322_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_641_1 ) ) fail(__LINE__);
    if( TEST_UNSIGNED( en1_642_1 ) ) fail(__LINE__);
#if !defined( __I86__ )
    if( TEST_UNSIGNED( en1_163_1 ) ) fail(__LINE__);
#endif

// test unsigned values

#if defined( __I86__ )
    if( TEST_SIGNED( en1_163_1 ) ) fail(__LINE__);
#endif
    if( TEST_SIGNED( en1_323_1 ) ) fail(__LINE__);
    if( TEST_SIGNED( en1_643_1 ) ) fail(__LINE__);

    _PASS;
}
