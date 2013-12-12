.func _m_pcmpgtb
#include <mmintrin.h>
__m64 _m_pcmpgtb(__m64 *m1, __m64 *m2);
.synop end
.desc begin
If the respective signed bytes of
.arg m1
are greater than the respective signed bytes of
.arg m2
.ct , the respective bytes of the result are set to all ones, otherwise they
are set to all zeros.
.desc end
.return begin
The result of comparing the packed signed bytes of two 64-bit
multimedia values is returned as a sequence of bytes (0xff for greater
than, 0x00 for not greater than).
.return end
.see begin
.im seemmcmp &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_BYTES "%2.2x %2.2x %2.2x %2.2x " \
                 "%2.2x %2.2x %2.2x %2.2x"
.exmp break
__m64   a;
__m64   b = { 0x0004000300020001 };
__m64   c = { 0xff7fff800080007f };

void main()
  {
    a = _m_pcmpgtb( b, c );
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
m1=00 04 00 03 00 02 00 01
m2=ff 7f ff 80 00 80 00 7f
mm=ff 00 ff ff 00 ff 00 00
.exmp end
.class Intel
.system
