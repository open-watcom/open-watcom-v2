#include "fail.h"
#include <limits.h>

#ifdef __WATCOM_INT64__

#ifdef __WATCOMC__
#define I8 char
#define I16 short
#define I32 long
#else
#define I8 __int8
#define I16 __int16
#define I32 __int32
#endif

int foo( signed I8 x ) {
    return x + 1;
}
int foo( unsigned I8 x ) {
    return x + 2;
}
int foo( signed I16 x ) {
    return x + 3;
}
int foo( unsigned I16 x ) {
    return x + 4;
}
signed int foo( signed int x ) {
    return x + 5;
}
unsigned int foo( unsigned int x ) {
    return x + 6;
}
signed I32 foo( signed I32 x ) {
    return x + 7;
}
unsigned I32 foo( unsigned I32 x ) {
    return x + 8;
}
signed __int64 foo( signed __int64 x ) {
    return x + 9;
}
unsigned __int64 foo( unsigned __int64 x ) {
    return x + 10;
}

int main() {
    //__asm int 3;
    #define test( c, i )	(( foo( c ) != ( c + i ) ) && ( printf("%u\n",foo(c)-c), 1 ))
    if( test( 6i8, 1 ) ) _fail;
    if( test( 0x7i8, 1 ) ) _fail;
    if( test( 6ui8, 2 ) ) _fail;
    if( test( 0x7Ui8, 2 ) ) _fail;
#if INT_MAX < 65536
    if( test( 65536i8, 7 ) ) _fail;
    if( test( 0x10000i8, 7 ) ) _fail;
    if( test( 65536ui8, 7 ) ) _fail;
    if( test( 0x10000Ui8, 7 ) ) _fail;
#else
    if( test( 65536i8, 5 ) ) _fail;
    if( test( 0x10000i8, 5 ) ) _fail;
    if( test( 65536ui8, 5 ) ) _fail;
    if( test( 0x10000Ui8, 5 ) ) _fail;
#endif
    if( test( 6i16, 3 ) ) _fail;
    if( test( 0x7i16, 3 ) ) _fail;
    if( test( 6ui16, 4 ) ) _fail;
    if( test( 0x7Ui16, 4 ) ) _fail;
#if INT_MAX < 65536
    if( test( 65536i16, 7 ) ) _fail;
    if( test( 0x10000i16, 7 ) ) _fail;
    if( test( 65536ui16, 7 ) ) _fail;
    if( test( 0x10000Ui16, 7 ) ) _fail;
#else
    if( test( 65536i16, 5 ) ) _fail;
    if( test( 0x10000i16, 5 ) ) _fail;
    if( test( 65536ui16, 5 ) ) _fail;
    if( test( 0x10000Ui16, 5 ) ) _fail;
#endif
    if( test( 6i32, 7 ) ) _fail;
    if( test( 0x7i32, 7 ) ) _fail;
    if( test( 6ui32, 8 ) ) _fail;
    if( test( 0x7Ui32, 8 ) ) _fail;
    if( test( 65536i32, 7 ) ) _fail;
    if( test( 0x10000i32, 7 ) ) _fail;
    if( test( 65536ui32, 8 ) ) _fail;
    if( test( 0x10000Ui32, 8 ) ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
