.func _m_packuswb
#include <mmintrin.h>
__m64 _m_packuswb(__m64 *m1, __m64 *m2);
.synop end
.desc begin
Convert signed packed words into unsigned packed bytes by packing
(with unsigned saturation) the low-order bytes of the signed word
elements from
.arg m1
and
.arg m2
into the respective unsigned bytes of the result.
If the signed values in the word elements of
.arg m1
and
.arg m2
are too large to be represented in an unsigned byte, the result
elements are clamped to 0xff.
.millust begin
            m2                         m1
-------------------------  -------------------------
|b7 b6|b5 b4|b3 b2|b1 b0|  |b7 b6|b5 b4|b3 b2|b1 b0|
-------------------------  -------------------------
   |     |     |     |        |     |     |     |
   |     |     |     `--.  .--'     |     |     |
   |     |     `-----.  |  |  .-----'     |     |
   |     `--------.  |  |  |  |  .--------'     |
   `-----------.  |  |  |  |  |  |  .-----------'
               |  |  |  |  |  |  |  |
               V  V  V  V  V  V  V  V
              -------------------------
              |b7|b6|b5|b4|b3|b2|b1|b0|
              -------------------------
                    result
.millust end
.desc end
.return begin
The result of packing, with unsigned saturation, 16-bit signed words
into 8-bit unsigned bytes is returned.
.return end
.see begin
.im seemmpck &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_BYTES "%2.2x %2.2x %2.2x %2.2x " \
                 "%2.2x %2.2x %2.2x %2.2x"
#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
#define AS_DWORDS "%8.8lx %8.8lx"
.exmp break
__m64   a;
__m64   b = { 0x0004000300020001 };
__m64   c = { 0xff7fff800080007f };
.exmp break
void main()
  {
    a = _m_packuswb( b, c );
    printf( "m2="AS_WORDS" "
            "m1="AS_WORDS"\n"
            "mm="AS_BYTES"\n",
        c._16[3], c._16[2], c._16[1], c._16[0],
        b._16[3], b._16[2], b._16[1], b._16[0],
        a._8[7], a._8[6], a._8[5], a._8[4],
        a._8[3], a._8[2], a._8[1], a._8[0] );
  }
.exmp output
m2=ff7f ff80 0080 007f m1=0004 0003 0002 0001
mm=00 00 80 7f 04 03 02 01
.exmp end
.class Intel
.system
