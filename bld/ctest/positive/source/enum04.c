#include "fail.h"
#include <stdio.h>

typedef enum {
    en81_3 = -0x7F,
    en81_1 = 0x7F,
    en81_2 = -0x7F
} enum_s8;

typedef enum {
    en82_3 = 0x7F,
    en82_1 = 0xFF,
    en82_2 = 0x7F
} enum_u8;

typedef enum {
    en161_3 = -0x7F,
    en161_1 = 0xFF,
    en161_2 = -0x7F
} enum_s16;

typedef enum {
    en162_3 = 0x7FFF,
    en162_1 = 0xFFFF,
    en162_2 = 0x7FFF
} enum_u16;

typedef enum {
    en321_3 = -0x7FFF,
    en321_1 = 0xFFFF,
    en321_2 = -0x7FFF
} enum_s32;

typedef enum {
    en322_3 = 0x7FFFFFFF,
    en322_1 = 0xFFFFFFFF,
    en322_2 = 0x7FFFFFFF
} enum_u32;

typedef enum {
    en641_3 = -0x7FFFFFFF,
    en641_1 = 0xFFFFFFFF,
    en641_2 = -0x7FFFFFFF
} enum_s64;

typedef enum {
    en642_3 = 0x7FFFFFFFFFFFFFFF,
    en642_1 = 0xFFFFFFFFFFFFFFFF,
    en642_2 = 0x7FFFFFFFFFFFFFFF
} enum_u64;

typedef enum {
    en1 = 0x40,
    en2 = 0x80,
    en3 = en1 | en2
} enum_or1;

typedef enum {
    en4 = 0x4000,
    en5 = 0x8000,
    en6 = en4 | en5
} enum_or2;

typedef enum {
    en7 = 0x40000000,
    en8 = 0x80000000,
    en9 = en7 | en8
} enum_or3;

#define const8 (1 << 6)
#define const16 ((1 << 14) | const8 )
#define const32 ((1I32 << 30) | const16 )
#define const64 ((1I64 << 62) | const32 )

enum_u8 var8 = const8;
enum_u16 var16 = const16;
enum_u32 var32 = const32;
enum_u64 var64 = const64;

int main()
{
    enum_s8 test8s = const8;
    enum_u8 test8u = const8;
    enum_s16 test16s = const16;
    enum_u16 test16u = const16;
    enum_s32 test32s = const32;
    enum_u32 test32u = const32;
    enum_s64 test64s = const64;
    enum_u64 test64u = const64;
    long long test3;
    int size;

    size = sizeof(enum_or1);
#ifdef __SW_EI
    if( size != sizeof(int) ) fail(__LINE__);
#else
    if( size != sizeof(unsigned char) ) fail(__LINE__);
#endif
    test3 = en3;
    if( test3 != 0xc0U ) fail(__LINE__);
    size = sizeof(enum_or2);
#ifdef __SW_EI
    if( size != sizeof(int) ) fail(__LINE__);
#else
    if( size != sizeof(unsigned short) ) fail(__LINE__);
#endif
    test3 = en6;
    if( test3 != 0xc000U ) fail(__LINE__);
    size = sizeof(enum_or3);
    if( size != sizeof(unsigned long) ) fail(__LINE__);
    test3 = en9;
    if( test3 != 0xc0000000UL ) fail(__LINE__);

    test3 = test8s + test8u;
    if( test3 != var8 * 2 ) fail(__LINE__);
    test3 = test8s | test8u;
    if( test3 != var8 ) fail(__LINE__);
    test3 = test8s & test8u;
    if( test3 != var8 ) fail(__LINE__);

    test3 = test16s + test16u;
    if( test3 != var16 * 2 ) fail(__LINE__);
    test3 = test16s | test16u;
    if( test3 != var16 ) fail(__LINE__);
    test3 = test16s & test16u;
    if( test3 != var16 ) fail(__LINE__);

    test3 = test32s + test32u;
    if( test3 != var32 * 2 ) fail(__LINE__);
    test3 = test32s | test32u;
    if( test3 != var32 ) fail(__LINE__);
    test3 = test32s & test32u;
    if( test3 != var32 ) fail(__LINE__);

    test3 = test64s + test64u;
    if( test3 != var64 * 2 ) fail(__LINE__);
    test3 = test64s | test64u;
    if( test3 != var64 ) fail(__LINE__);
    test3 = test64s & test64u;
    if( test3 != var64 ) fail(__LINE__);

    _PASS;
}
