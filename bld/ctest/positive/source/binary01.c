/* c23 feature, also available as extension in older c versions */
/* tested in c23 mode. extensions disabled */

#include "fail.h"

#include <stdint.h>

static int b_zero = 0b0;
static int b_one =  0b1;
static int b_two = 0b10;
static int b_fortytwo   =   0b101010;
static int b_00fortytwo = 0b00101010; /* leading zeros */
/* 32bit bit value helper        84218421842184218421842184218421 */
static int32_t  b_int32_7f   = 0b01111111011111110111111101111111;
static int32_t  b_int32_80   = 0b10000000100000001000000010000000;
static uint32_t b_uint32_max = 0b11111111111111111111111111111111;

static int64_t b_int64_11 =                                   0b100000000000000000000000000000001;
/* 64bit bit value helper        8421842184218421842184218421842184218421842184218421842184218421 */
static int64_t b_int64_7f =    0b0111111101111111011111110111111101111111011111110111111101111111;
static uint64_t b_uint64_max = 0b1111111111111111111111111111111111111111111111111111111111111111;


/* Keep the optimizer away from our test code */
volatile int extra = 0;


int get_0(void)
{
    return b_zero + extra;
}

int get_1(void)
{
    return b_one + extra;
}

int get_2(void)
{
    return b_two * 0b1  + extra;
}

int get_42(void)
{
    return b_fortytwo + extra;
}

int get_0042(void)
{
    return b_00fortytwo + extra;
}

int32_t get_7f(void)
{
    return b_int32_7f + extra;
}

int32_t get_80(void)
{
    return b_int32_80 + extra;
}

uint32_t get_max(void)
{
    return b_uint32_max + extra;
}

int64_t get64_11(void)
{
    return b_int64_11 + extra;
}

int64_t get64_7f(void)
{
    return b_int64_7f + extra;
}

uint64_t get64_max(void)
{
    return b_uint64_max + extra;
}


int main(int argc, char * argv[])
{
    /* Keep the optimizer away from our test code */
    if(( argc == 42 ) && ( argv[0] == argv[1] )) {
        extra++;
    }

    if( get_0() != 0 ) fail(__LINE__);
    if( get_1() != 1 ) fail(__LINE__);
    if( get_2() != 2 ) fail(__LINE__);

    if( get_42()   != 42 ) fail(__LINE__);
    if( get_0042() != 42 ) fail(__LINE__);

    if( get_7f()  != 0x7f7f7f7f ) fail(__LINE__);
    if( get_80()  != 0x80808080 ) fail(__LINE__);
    if( get_max() != 0xffffffff ) fail(__LINE__);

    if( get64_11()  !=        0x100000001 ) fail(__LINE__);
    if( get64_7f()  != 0x7f7f7f7f7f7f7f7f ) fail(__LINE__);
    if( get64_max() != 0xffffffffffffffff ) fail(__LINE__);

    _PASS;
}
