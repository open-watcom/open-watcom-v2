#include "fail.h"
#include <limits.h>

int fooi___( int x ) {
    return x + 1;
}
unsigned foou___( unsigned x ) {
    return x + 2;
}
long fool___( long x ) {
    return x + 3;
}
unsigned long fooul__( unsigned long x ) {
    return x + 4;
}
#ifdef __WATCOM_INT64__
__int64 fooi64_( __int64 x ) {
    return x + 5;
}
unsigned __int64 fooui64( unsigned __int64 x ) {
    return x + 6;
}
__int64 bar( __int64 x ) {
    return x;
}
#endif

int main() {
    #define testi___( c, i )    fooi___( c ) != ( c + i )
    #define testu___( c, i )    foou___( c ) != ( c + i )
    #define testl___( c, i )    fool___( c ) != ( c + i )
    #define testul__( c, i )    fooul__( c ) != ( c + i )
    #define testi64_( c, i )    fooi64_( c ) != ( c + i )
    #define testui64( c, i )    fooui64( c ) != ( c + i )
    if( testi___( 0, 1 ) ) _fail;
    if( testu___( 0u, 2 ) ) _fail;
    if( testu___( 0U, 2 ) ) _fail;
    if( testul__( 0ul, 4 ) ) _fail;
    if( testul__( 0uL, 4 ) ) _fail;
    if( testul__( 0Ul, 4 ) ) _fail;
    if( testul__( 0UL, 4 ) ) _fail;
    if( testul__( 0lu, 4 ) ) _fail;
    if( testul__( 0Lu, 4 ) ) _fail;
    if( testul__( 0lU, 4 ) ) _fail;
    if( testul__( 0LU, 4 ) ) _fail;
#ifdef __WATCOM_INT64__
    if( testi64_( 0i64, 5 ) ) _fail;
    if( testi64_( 0I64, 5 ) ) _fail;
    if( testi64_( 0ll, 5 ) ) _fail;
    if( testi64_( 0LL, 5 ) ) _fail;
    if( testi64_( 0Ll, 5 ) ) _fail;
    if( testi64_( 0lL, 5 ) ) _fail;
    if( testui64( 0ui64, 6 ) ) _fail;
    if( testui64( 0uI64, 6 ) ) _fail;
    if( testui64( 0Ui64, 6 ) ) _fail;
    if( testui64( 0UI64, 6 ) ) _fail;
    if( testui64( 0ull, 6 ) ) _fail;
    if( testui64( 0uLl, 6 ) ) _fail;
    if( testui64( 0ulL, 6 ) ) _fail;
    if( testui64( 0uLL, 6 ) ) _fail;
    if( testui64( 0Ull, 6 ) ) _fail;
    if( testui64( 0ULl, 6 ) ) _fail;
    if( testui64( 0UlL, 6 ) ) _fail;
    if( testui64( 0ULL, 6 ) ) _fail;
    if( testi64_( 9999999999, 5 ) ) _fail;
    if( testi64_( 9999999999l, 5 ) ) _fail;
    if( testi64_( 9999999999L, 5 ) ) _fail;
    if( testui64( 9999999999U, 6 ) ) _fail;
    if( testui64( 9999999999Ul, 6 ) ) _fail;
    if( testui64( 9999999999UL, 6 ) ) _fail;
    if( testui64( 9999999999u, 6 ) ) _fail;
    if( testui64( 9999999999ul, 6 ) ) _fail;
    if( testui64( 9999999999uL, 6 ) ) _fail;
    if( testi64_( 0x9999999999, 5 ) ) _fail;
    if( testi64_( 0x9999999999l, 5 ) ) _fail;
    if( testi64_( 0x9999999999L, 5 ) ) _fail;
    if( testui64( 0x9999999999U, 6 ) ) _fail;
    if( testui64( 0x9999999999Ul, 6 ) ) _fail;
    if( testui64( 0x9999999999UL, 6 ) ) _fail;
    if( testui64( 0x9999999999u, 6 ) ) _fail;
    if( testui64( 0x9999999999ul, 6 ) ) _fail;
    if( testui64( 0x9999999999uL, 6 ) ) _fail;
#endif
    if( testi___( CHAR_MIN, 1 ) ) _fail;
    if( testi___( CHAR_MAX, 1 ) ) _fail;
    if( testi___( SCHAR_MIN, 1 ) ) _fail;
    if( testi___( SCHAR_MAX, 1 ) ) _fail;
    if( testu___( UCHAR_MAX, 2 ) ) _fail;
    if( testi___( SHRT_MIN, 1 ) ) _fail;
    if( testi___( SHRT_MAX, 1 ) ) _fail;
    if( testu___( USHRT_MAX, 2 ) ) _fail;
    if( testi___( INT_MIN, 1 ) ) _fail;
    if( testi___( INT_MAX, 1 ) ) _fail;
    if( testu___( UINT_MAX, 2 ) ) _fail;
    if( testl___( LONG_MIN, 3 ) ) _fail;
    if( testl___( LONG_MAX, 3 ) ) _fail;
    if( testul__( ULONG_MAX, 4 ) ) _fail;
#ifdef __WATCOM_INT64__
    if( testi64_( LONGLONG_MIN, 5 ) ) _fail;
    if( testi64_( LONGLONG_MAX, 5 ) ) _fail;
    if( testui64( ULONGLONG_MAX, 6 ) ) _fail;
    if( testi64_( LLONG_MIN, 5 ) ) _fail;
    if( testi64_( LLONG_MAX, 5 ) ) _fail;
    if( testui64( ULLONG_MAX, 6 ) ) _fail;
    if( testi64_( _I64_MIN, 5 ) ) _fail;
    if( testi64_( _I64_MAX, 5 ) ) _fail;
    if( testui64( _UI64_MAX, 6 ) ) _fail;
#endif
    if( testi___( 0, 1 ) ) _fail;
#if INT_MAX < 65536
    if( testi___( 32766, 1 ) ) _fail;
    if( testi___( 32767, 1 ) ) _fail;
    if( testl___( 32766l, 3 ) ) _fail;
    if( testl___( 32767L, 3 ) ) _fail;
    if( testl___( 32768, 3 ) ) _fail;
    if( testu___( 32768U, 2 ) ) _fail;
    if( testu___( 32768u, 2 ) ) _fail;
    if( testl___( 2147483646, 3 ) ) _fail;
    if( testl___( 2147483647, 3 ) ) _fail;
    if( testl___( 2147483646l, 3 ) ) _fail;
    if( testl___( 2147483647L, 3 ) ) _fail;
    if( testul__( 2147483648, 4 ) ) _fail;
    if( testul__( 2147483648U, 4 ) ) _fail;
    if( testul__( 2147483648u, 4 ) ) _fail;
    if( testul__( 4294967294, 4 ) ) _fail;
    if( testul__( 4294967294u, 4 ) ) _fail;
    if( testul__( 4294967294U, 4 ) ) _fail;
    if( testul__( 4294967295, 4 ) ) _fail;
    if( testul__( 4294967295u, 4 ) ) _fail;
    if( testul__( 4294967295U, 4 ) ) _fail;
#elif INT_MAX < 4294967295U
    if( testi___( 32766, 1 ) ) _fail;
    if( testi___( 32767, 1 ) ) _fail;
    if( testl___( 32766l, 3 ) ) _fail;
    if( testl___( 32767L, 3 ) ) _fail;
    if( testi___( 32768, 1 ) ) _fail;
    if( testu___( 32768U, 2 ) ) _fail;
    if( testu___( 32768u, 2 ) ) _fail;
    if( testi___( 2147483646, 1 ) ) _fail;
    if( testi___( 2147483647, 1 ) ) _fail;
    if( testl___( 2147483646l, 3 ) ) _fail;
    if( testl___( 2147483647L, 3 ) ) _fail;
    if( testul__( 2147483648, 4 ) ) _fail;
    if( testu___( 2147483648U, 2 ) ) _fail;
    if( testu___( 2147483648u, 2 ) ) _fail;
    if( testul__( 4294967294, 4 ) ) _fail;
    if( testu___( 4294967294u, 2 ) ) _fail;
    if( testu___( 4294967294U, 2 ) ) _fail;
    if( testul__( 4294967295, 4 ) ) _fail;
    if( testu___( 4294967295u, 2 ) ) _fail;
    if( testu___( 4294967295U, 2 ) ) _fail;
#else
#error int is larger than expected
#endif
#ifdef __WATCOM_INT64__
    if( testi64_( 4294967296, 5 ) ) _fail;
    if( testui64( 4294967296u, 6 ) ) _fail;
    if( testui64( 4294967296U, 6 ) ) _fail;
    /* Test for bug in Open Watcom 1.2 scanner */
    if( bar( 12345LL ) != 12345 ) _fail;
    if( bar( 12345ULL ) != 12345 ) _fail;
#endif
    _PASS;
}
