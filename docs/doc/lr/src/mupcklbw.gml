.func _m_punpcklbw
#include <mmintrin.h>
__m64 _m_punpcklbw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The &func function performs an interleaved unpack of the
low-order data elements of
.arg m1
and
.arg m2
.ct .li .
It ignores the high-order bytes.
When unpacking from a memory operand, 32 bits are accessed and all are
utilized by the instruction.
By choosing
.arg m1
or
.arg m2
to be zero, an unpacking of byte elements into word elements
is performed.
.millust begin
            m2                         m1
-------------------------  -------------------------
|           |b3|b2|b1|b0|  |b7|b6|b5|b4|b3|b2|b1|b0|
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
The result of the interleaved unpacking of the low-order bytes of two
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
__m64   b = { 0x000200013478bcf0 };
__m64   c = { 0x0080007f12569ade };
.exmp break
void main()
  {
    a = _m_punpcklbw( b, c );
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
m2=00 80 00 7f 12 56 9a de m1=00 02 00 01 34 78 bc f0
mm=12 34 56 78 9a bc de f0
.exmp end
.class Intel
.system
