#include <stdio.h>
#include <mmintrin.h>

#define AS_BYTES "%2.2x %2.2x %2.2x %2.2x " \
                 "%2.2x %2.2x %2.2x %2.2x"
#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
#define AS_DWORDS "%8.8lx %8.8lx"

__m64   a;
__m64   b = { 0x0000567800001234 };
__m64   c = { 0xfffffffe00010101 };

void main()
  {
    a = _m_packssdw( b, c );
    printf( "m2="AS_DWORDS" "
            "m1="AS_DWORDS"\n"
            "mm="AS_WORDS"\n",
        c._32[1], c._32[0],
        b._32[1], b._32[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
