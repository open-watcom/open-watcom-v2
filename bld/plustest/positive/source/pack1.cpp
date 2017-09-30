// 95/07/18 -- J.W.Welch        -- AXP conversion

#include "fail.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#if defined( __AXP__ )
#define check( intel, c32, c16, axp )           axp
#elif defined( INTEL_ABI )
#define check( intel, c32, c16, axp )           intel
#elif defined( __386__ )
#define check( intel, c32, c16, axp )           c32
#else
#define check( intel, c32, c16, axp )           c16
#endif

void verify( size_t gen, size_t check, unsigned line )
{
    printf( "failure on line %u (%u should be %u)\n", line, gen, check );
    fail( line );
}

#define test_off( STRUCT, FIELD, OFFSET ) \
        if( offsetof( STRUCT, FIELD ) != OFFSET ) \
            verify( offsetof( STRUCT, FIELD ), OFFSET, __LINE__ );

#define test_size( STRUCT, SIZE ) \
        if( sizeof( STRUCT ) != SIZE ) \
            verify( sizeof( STRUCT ), SIZE, __LINE__ );

#pragma pack(1);

typedef struct S1 {
    char c;
} S1;

#pragma pack(4);

typedef struct S2 {
    char c;
    char d;
    short s;
    long n;
} S2;

#pragma pack(2);

typedef struct S3 {
    char c;
    short s;
} S3;

#pragma pack(4);

typedef struct S4 {
    char c;
    double d;
    short s;
} S4;

typedef union S5 {
    char c;
    short s;
    int j;
} S5;

typedef struct S6 {
    int j : 5;
    int k : 6;
    int m : 7;
} S6;

typedef struct S7 {
    short s : 9;
    int j : 9;
    char c;
    short t : 9;
    short u : 9;
    char d;
} S7;

#pragma pack(2);

typedef struct S8 {
    char c;
    short s : 8;
} S8;

typedef union S9 {
    char c;
    short s : 8;
} S9;

#pragma pack(1);

typedef struct S10 {
    char c;
    int : 0;
    char d;
    short : 9;
    char e;
    char : 0;
} S10;

typedef struct S11 {
    char x : 1;
    int : 0;
    char c;
} S11;

typedef struct S12 {
    unsigned char x : 1;
    signed char y : 1;
    char c;
    unsigned char w : 1;
    signed char z : 1;
    int : 0;
    char d;
} S12;

typedef struct S13 {
    char c;
    int a : 1;
    int : 0;
    char d;
    int : 0;
    char e;
    int f : 1;
    int : 0;
    int : 0;
    char g;
} S13;

#pragma pack(2);

typedef struct S14 {
    char c;
    S5 u;
} S14;

#pragma pack(4)

typedef struct S15 {
    long double a;
    long b;
    char c;
    long double d;
} S15;

#pragma pack(16)

typedef struct S16 {
    char c;
    long double a;
    long double b;
} S16;

#pragma pack(2)

typedef struct S17 {
    char c;
} S17;
#ifdef __cplusplus
struct D17 : S17 {
};
#endif

#pragma pack();

int main( void )
{
    // 3-3
    test_off( S1, c, check( 0, 0, 0, 0 ) );
    test_size( S1, check( 1, 1, 1, 1 ) );
    // 3-4
    test_off( S2, c, check( 0, 0, 0, 0 ) );
    test_off( S2, d, check( 1, 1, 1, 1 ) );
    test_off( S2, s, check( 2, 2, 2, 2 ) );
    test_off( S2, n, check( 4, 4, 4, 4 ) );
    test_size( S2, check( 8, 8, 8, 8 ) );
    test_off( S3, c, check( 0, 0, 0, 0 ) );
    test_off( S3, s, check( 2, 2, 2, 2 ) );
    test_size( S3, check( 4, 4, 4, 4 ) );
    test_off( S4, c, check( 0, 0, 0, 0 ) );
    test_off( S4, d, check( 4, 4, 4, 4 ) );
    test_off( S4, s, check( 12, 12, 12, 12 ) );
    test_size( S4, check( 16, 16, 16, 16 ) );
    // 3-5
    test_off( S5, c, check( 0, 0, 0, 0 ) );
    test_off( S5, s, check( 0, 0, 0, 0 ) );
    test_off( S5, j, check( 0, 0, 0, 0 ) );
    test_size( S5, check( 4, 4, 2, 4 ) );
    // 3-6
    test_size( S6, check( 4, 4, 4, 4 ) );
    // 3-7
    test_off( S7, c, check( 3, 8, 4, 8 ) );
    test_off( S7, d, check( 8, 14, 10, 14 ) );
    test_size( S7, check( 12, 16, 12, 16 ) );
    test_off( S8, c, check( 0, 0, 0, 0 ) );
    test_size( S8, check( 2, 4, 4, 4 ) );
    test_off( S9, c, check( 0, 0, 0, 0 ) );
    test_size( S9, check( 2, 2, 2, 2 ) );
    // 3-8
    test_off( S10, c, check( 0, 0, 0, 0 ) );
    test_off( S10, d, check( 4, 4, 2, 4 ) );
    test_off( S10, e, check( 8, 7, 5, 7 ) );
    test_size( S10, check( 9, 8, 6, 8 ) );
    // extra
    test_off( S11, c, check( 4, 4, 2, 4 ) );
    test_size( S11, check( 5, 5, 3, 5 ) );
    test_off( S12, c, check( 1, 1, 1, 1 ) );
    test_off( S12, d, check( 4, 4, 4, 4 ) );
    test_size( S12, check( 5, 5, 5, 5 ) );
    test_off( S13, c, check( 0, 0, 0, 0 ) );
    test_off( S13, d, check( 5, 5, 3, 5 ) );
    test_off( S13, e, check( 8, 8, 4, 8 ) );
    test_off( S13, g, check( 16, 16, 8, 16 ) );
    test_size( S13, check( 17, 17, 9, 17 ) );
    test_off( S14, c, check( 0, 0, 0, 0 ) );
    test_off( S14, u, check( 4, 2, 2, 2 ) );
    test_size( S14, check( 8, 6, 4, 6 ) );
    test_off( S15, b, check( 8, 8, 8, 8 ) );
    test_off( S15, c, check( 12, 12, 12, 12 ) );
    test_off( S15, d, check( 16, 16, 16, 16 ) );
    test_size( S15, check( 24, 24, 24, 24 ) );
    test_off( S16, a, check( 8, 8, 8, 8 ) );
    test_off( S16, b, check( 16, 16, 16, 16 ) );
    test_size( S16, check( 24, 24, 24, 24 ) );
    test_off( S17, c, check( 0, 0, 0, 0 ) );
    test_size( S17, check( 1, 1, 1, 1 ) );
#ifdef __cplusplus
    test_off( D17, c, check( 0, 0, 0, 0 ) );
#if __WATCOM_CPLUSPLUS__ >= 1000
    test_size( D17, check( sizeof(S17), sizeof(S17), sizeof(S17), sizeof(S17) ) );
    test_size( D17, check( 1, 1, 1, 1 ) );
#else
    test_size( D17, check( 4, 4, 2, 4 ) );
#endif
#endif

    _PASS;
}
