#include "fail.h"
#include <limits.h>

int foo( int x ) {
    return x + 1;
}
unsigned foo( unsigned x ) {
    return x + 2;
}
long foo( long x ) {
    return x + 3;
}
unsigned long foo( unsigned long x ) {
    return x + 4;
}
#ifdef __WATCOM_INT64__
__int64 foo( __int64 x ) {
    return x + 5;
}
unsigned __int64 foo( unsigned __int64 x ) {
    return x + 6;
}
#endif

int main() {
    #define test( c, i )	foo( c ) != ( c + i )
    if( test( 0, 1 ) ) _fail;
    if( test( 0u, 2 ) ) _fail;
    if( test( 0U, 2 ) ) _fail;
    if( test( 0ul, 4 ) ) _fail;
    if( test( 0uL, 4 ) ) _fail;
    if( test( 0Ul, 4 ) ) _fail;
    if( test( 0UL, 4 ) ) _fail;
    if( test( 0lu, 4 ) ) _fail;
    if( test( 0Lu, 4 ) ) _fail;
    if( test( 0lU, 4 ) ) _fail;
    if( test( 0LU, 4 ) ) _fail;
#ifdef __WATCOM_INT64__
    if( test( 0i64, 5 ) ) _fail;
    if( test( 0I64, 5 ) ) _fail;
    if( test( 0ui64, 6 ) ) _fail;
    if( test( 0uI64, 6 ) ) _fail;
    if( test( 0Ui64, 6 ) ) _fail;
    if( test( 0UI64, 6 ) ) _fail;
    if( test( 9999999999, 5 ) ) _fail;
    if( test( 9999999999l, 5 ) ) _fail;
    if( test( 9999999999L, 5 ) ) _fail;
    if( test( 9999999999U, 6 ) ) _fail;
    if( test( 9999999999Ul, 6 ) ) _fail;
    if( test( 9999999999UL, 6 ) ) _fail;
    if( test( 9999999999u, 6 ) ) _fail;
    if( test( 9999999999ul, 6 ) ) _fail;
    if( test( 9999999999uL, 6 ) ) _fail;
    if( test( 0x9999999999, 5 ) ) _fail;
    if( test( 0x9999999999l, 5 ) ) _fail;
    if( test( 0x9999999999L, 5 ) ) _fail;
    if( test( 0x9999999999U, 6 ) ) _fail;
    if( test( 0x9999999999Ul, 6 ) ) _fail;
    if( test( 0x9999999999UL, 6 ) ) _fail;
    if( test( 0x9999999999u, 6 ) ) _fail;
    if( test( 0x9999999999ul, 6 ) ) _fail;
    if( test( 0x9999999999uL, 6 ) ) _fail;
#endif
    if( test( CHAR_MIN, 1 ) ) _fail;
    if( test( CHAR_MAX, 1 ) ) _fail;
    if( test( SCHAR_MIN, 1 ) ) _fail;
    if( test( SCHAR_MAX, 1 ) ) _fail;
    if( test( UCHAR_MAX, 2 ) ) _fail;
    if( test( SHRT_MIN, 1 ) ) _fail;
    if( test( SHRT_MAX, 1 ) ) _fail;
    if( test( USHRT_MAX, 2 ) ) _fail;
    if( test( INT_MIN, 1 ) ) _fail;
    if( test( INT_MAX, 1 ) ) _fail;
    if( test( UINT_MAX, 2 ) ) _fail;
    if( test( LONG_MIN, 3 ) ) _fail;
    if( test( LONG_MAX, 3 ) ) _fail;
    if( test( ULONG_MAX, 4 ) ) _fail;
#ifdef __WATCOM_INT64__
    if( test( LONGLONG_MIN, 5 ) ) _fail;
    if( test( LONGLONG_MAX, 5 ) ) _fail;
    if( test( ULONGLONG_MAX, 6 ) ) _fail;
    if( test( _I64_MIN, 5 ) ) _fail;
    if( test( _I64_MAX, 5 ) ) _fail;
    if( test( _UI64_MAX, 6 ) ) _fail;
#endif
    if( test( 0, 1 ) ) _fail;
#if INT_MAX < 65536
    if( test( 32766, 1 ) ) _fail;
    if( test( 32767, 1 ) ) _fail;
    if( test( 32766l, 3 ) ) _fail;
    if( test( 32767L, 3 ) ) _fail;
    if( test( 32768, 3 ) ) _fail;
    if( test( 32768U, 2 ) ) _fail;
    if( test( 32768u, 2 ) ) _fail;
    if( test( 2147483646, 3 ) ) _fail;
    if( test( 2147483647, 3 ) ) _fail;
    if( test( 2147483646l, 3 ) ) _fail;
    if( test( 2147483647L, 3 ) ) _fail;
    if( test( 2147483648, 4 ) ) _fail;
    if( test( 2147483648U, 4 ) ) _fail;
    if( test( 2147483648u, 4 ) ) _fail;
    if( test( 4294967294, 4 ) ) _fail;
    if( test( 4294967294u, 4 ) ) _fail;
    if( test( 4294967294U, 4 ) ) _fail;
    if( test( 4294967295, 4 ) ) _fail;
    if( test( 4294967295u, 4 ) ) _fail;
    if( test( 4294967295U, 4 ) ) _fail;
#elif INT_MAX < 4294967295U
    if( test( 32766, 1 ) ) _fail;
    if( test( 32767, 1 ) ) _fail;
    if( test( 32766l, 3 ) ) _fail;
    if( test( 32767L, 3 ) ) _fail;
    if( test( 32768, 1 ) ) _fail;
    if( test( 32768U, 2 ) ) _fail;
    if( test( 32768u, 2 ) ) _fail;
    if( test( 2147483646, 1 ) ) _fail;
    if( test( 2147483647, 1 ) ) _fail;
    if( test( 2147483646l, 3 ) ) _fail;
    if( test( 2147483647L, 3 ) ) _fail;
    if( test( 2147483648, 4 ) ) _fail;
    if( test( 2147483648U, 2 ) ) _fail;
    if( test( 2147483648u, 2 ) ) _fail;
    if( test( 4294967294, 4 ) ) _fail;
    if( test( 4294967294u, 2 ) ) _fail;
    if( test( 4294967294U, 2 ) ) _fail;
    if( test( 4294967295, 4 ) ) _fail;
    if( test( 4294967295u, 2 ) ) _fail;
    if( test( 4294967295U, 2 ) ) _fail;
#else
#error int is larger than expected
#endif
#ifdef __WATCOM_INT64__
    if( test( 4294967296, 5 ) ) _fail;
    if( test( 4294967296u, 6 ) ) _fail;
    if( test( 4294967296U, 6 ) ) _fail;
#endif
    _PASS;
}
