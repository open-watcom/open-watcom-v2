# Naive exerciser of <limits.h>

# wcl is a suitable alternative $(CC). An alternative will select 16-bit values.

!ifndef CC
CC = bwcl386
!endif

all : .symbolic
    $(CC) -fe=exercise -fo=exercise -zq <<
$#include <stdio.h>
$#include <limits.h>

int main( void )
{
    printf( "CHAR_BIT      = %ld\n", (long)CHAR_BIT );
    printf( "CHAR_MAX      = %ld\n", (long)CHAR_MAX );
    printf( "CHAR_MIN      = %ld\n", (long)CHAR_MIN );
    printf( "INT_MAX       = %ld\n", (long)INT_MAX );
    printf( "INT_MIN       = %ld\n", (long)INT_MIN );
#ifdef __WATCOM_INT64__
    printf( "LLONG_MAX     = %lld\n",       LLONG_MAX );
    printf( "LLONG_MIN     = %lld\n",       LLONG_MIN );
    printf( "LONGLONG_MAX  = %lld\n",       LONGLONG_MAX );
    printf( "LONGLONG_MIN  = %lld\n",       LONGLONG_MIN );
#endif
    printf( "LONG_MAX      = %ld\n", (long)LONG_MAX );
    printf( "LONG_MIN      = %ld\n", (long)LONG_MIN );
    printf( "MB_LEN_MAX    = %ld\n", (long)MB_LEN_MAX );
    printf( "SCHAR_MAX     = %ld\n", (long)SCHAR_MAX );
    printf( "SCHAR_MIN     = %ld\n", (long)SCHAR_MIN );
    printf( "SHRT_MAX      = %ld\n", (long)SHRT_MAX );
    printf( "SHRT_MIN      = %ld\n", (long)SHRT_MIN );
    printf( "UCHAR_MAX     = %ld\n", (long)UCHAR_MAX );
    printf( "UINT_MAX      = %u\n",        UINT_MAX );
#ifdef __WATCOM_INT64__
    printf( "ULLONG_MAX    = %llu\n",       ULLONG_MAX );
    printf( "ULONGLONG_MAX = %llu\n",       ULONGLONG_MAX );
#endif
    printf( "ULONG_MAX     = %lu\n",       ULONG_MAX );
    printf( "USHRT_MAX     = %ld\n", (long)USHRT_MAX );
    printf( "_I16_MAX      = %ld\n", (long)_I16_MAX );
    printf( "_I16_MIN      = %ld\n", (long)_I16_MIN );
    printf( "_I32_MAX      = %ld\n", (long)_I32_MAX );
    printf( "_I32_MIN      = %ld\n", (long)_I32_MIN );
#ifdef __WATCOM_INT64__
    printf( "_I64_MAX      = %lld\n",       _I64_MAX );
    printf( "_I64_MIN      = %lld\n",       _I64_MIN );
#endif
    printf( "_I8_MAX       = %ld\n", (long)_I8_MAX );
    printf( "_I8_MIN       = %ld\n", (long)_I8_MIN );
    printf( "_UI16_MAX     = %ld\n", (long)_UI16_MAX );
    printf( "_UI32_MAX     = %lu\n",       _UI32_MAX );
#ifdef __WATCOM_INT64__
    printf( "_UI64_MAX     = %llu\n",       _UI64_MAX );
#endif
    printf( "_UI8_MAX      = %ld\n", (long)_UI8_MAX );
    return( 0 );
}
<<
!ifeq CC bwcl386
    exercise | diff - <<
CHAR_BIT      = 8
CHAR_MAX      = 255
CHAR_MIN      = 0
INT_MAX       = 2147483647
INT_MIN       = -2147483648
LLONG_MAX     = 9223372036854775807
LLONG_MIN     = -9223372036854775808
LONGLONG_MAX  = 9223372036854775807
LONGLONG_MIN  = -9223372036854775808
LONG_MAX      = 2147483647
LONG_MIN      = -2147483648
MB_LEN_MAX    = 2
SCHAR_MAX     = 127
SCHAR_MIN     = -128
SHRT_MAX      = 32767
SHRT_MIN      = -32768
UCHAR_MAX     = 255
UINT_MAX      = 4294967295
ULLONG_MAX    = 18446744073709551615
ULONGLONG_MAX = 18446744073709551615
ULONG_MAX     = 4294967295
USHRT_MAX     = 65535
_I16_MAX      = 32767
_I16_MIN      = -32768
_I32_MAX      = 2147483647
_I32_MIN      = -2147483648
_I64_MAX      = 9223372036854775807
_I64_MIN      = -9223372036854775808
_I8_MAX       = 127
_I8_MIN       = -128
_UI16_MAX     = 65535
_UI32_MAX     = 4294967295
_UI64_MAX     = 18446744073709551615
_UI8_MAX      = 255

<<
!else
    exercise | diff - <<
CHAR_BIT      = 8
CHAR_MAX      = 255
CHAR_MIN      = 0
INT_MAX       = 32767
INT_MIN       = -32768
LLONG_MAX     = 9223372036854775807
LLONG_MIN     = -9223372036854775808
LONGLONG_MAX  = 9223372036854775807
LONGLONG_MIN  = -9223372036854775808
LONG_MAX      = 2147483647
LONG_MIN      = -2147483648
MB_LEN_MAX    = 2
SCHAR_MAX     = 127
SCHAR_MIN     = -128
SHRT_MAX      = 32767
SHRT_MIN      = -32768
UCHAR_MAX     = 255
UINT_MAX      = 65535
ULLONG_MAX    = 18446744073709551615
ULONGLONG_MAX = 18446744073709551615
ULONG_MAX     = 4294967295
USHRT_MAX     = 65535
_I16_MAX      = 32767
_I16_MIN      = -32768
_I32_MAX      = 2147483647
_I32_MIN      = -2147483648
_I64_MAX      = 9223372036854775807
_I64_MIN      = -9223372036854775808
_I8_MAX       = 127
_I8_MIN       = -128
_UI16_MAX     = 65535
_UI32_MAX     = 4294967295
_UI64_MAX     = 18446744073709551615
_UI8_MAX      = 255

<<
!endif
    rm -f exercise.*

clean: .symbolic
    rm -f exercise.*
