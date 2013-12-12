.func _m_punpckhbw
#include <mmintrin.h>
__m64 _m_punpckhbw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The &func function performs an interleaved unpack of the
high-order data elements of
.arg m1
and
.arg m2
.ct .li .
It ignores the low-order bytes.
When unpacking from a memory operand, the full 64-bit operand
is accessed from memory but only the high-order 32 bits are utilized.
By choosing
.arg m1
or
.arg m2
to be zero, an unpacking of byte elements into word elements
is performed.
.millust begin
            m2                         m1
-------------------------  -------------------------
|b7|b6|b5|b4|b3|b2|b1|b0|  |b7|b6|b5|b4|b3|b2|b1|b0|
-------------------------  -------------------------
 |  |  |  |                 |  |  |  |
 V  V  V  V                 V  V  V  V
 b7 b5 b3 b1                b6 b4 b2 b0

              -------------------------
              |b7|b6|b5|b4|b3|b2|b1|b0|
              -------------------------
                    result
.millust end
.desc end
.return begin
The result of the interleaved unpacking of the high-order bytes of two
multimedia values is returned.
.return end
.see begin
.im seemmupk &function.
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
.exmp break
void main()
  {
    a = _m_punpckhbw( b, c );
    printf( "m2="AS_BYTES" "
            "m1="AS_BYTES"\n"
            "mm="AS_BYTES"\n",
        c._8[7], c._8[6], c._8[5], c._8[4],
        c._8[3], c._8[2], c._8[1], c._8[0],
        b._8[7], b._8[6], b._8[5], b._8[4],
        b._8[3], b._8[2], b._8[1], b._8[0],
        a._8[7], a._8[6], a._8[5], a._8[4],
        a._8[3], a._8[2], a._8[1], a._8[0] );
  }
.exmp output
m2=ff 7f ff 80 00 80 00 7f m1=00 04 00 03 00 02 00 01
mm=ff 00 7f 04 ff 00 80 03
.exmp end
.class Intel
.system
