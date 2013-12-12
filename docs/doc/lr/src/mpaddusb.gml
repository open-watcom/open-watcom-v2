.func _m_paddusb
#include <mmintrin.h>
__m64 _m_paddusb(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The unsigned 8-bit bytes of
.arg m2
are added to the respective unsigned 8-bit bytes of
.arg m1
and the result is stored in memory.
Saturation occurs when a result exceeds the range of an unsigned byte.
In the case where a result is a byte larger than 0xff (overflow), it
is clamped to 0xff.
.desc end
.return begin
The result of adding the packed unsigned bytes, with saturation, of
two 64-bit multimedia values is returned.
.return end
.see begin
.im seemmadd &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_BYTES "%2.2x %2.2x %2.2x %2.2x " \
                 "%2.2x %2.2x %2.2x %2.2x"
.exmp break
__m64   a;
__m64   b = { 0x8aacceef02244668 };
__m64   c = { 0x76543211fedcba98 };

void main()
  {
    a = _m_paddusb( b, c );
    printf( "m1="AS_BYTES"\n"
            "m2="AS_BYTES"\n"
            "mm="AS_BYTES"\n",
        b._8[7], b._8[6], b._8[5], b._8[4],
        b._8[3], b._8[2], b._8[1], b._8[0],
        c._8[7], c._8[6], c._8[5], c._8[4],
        c._8[3], c._8[2], c._8[1], c._8[0],
        a._8[7], a._8[6], a._8[5], a._8[4],
        a._8[3], a._8[2], a._8[1], a._8[0] );
  }
.exmp output
m1=8a ac ce ef 02 24 46 68
m2=76 54 32 11 fe dc ba 98
mm=ff ff ff ff ff ff ff ff
.exmp end
.class Intel
.system
