.func _m_packssdw
.synop begin
#include <mmintrin.h>
__m64 _m_packssdw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
Convert signed packed double-words into signed packed words by packing
(with signed saturation) the low-order words of the signed double-word
elements from
.arg m1
and
.arg m2
into the respective signed words of the result.
If the signed values in the word elements of
.arg m1
and
.arg m2
are smaller than 0x8000, the result elements are clamped to 0x8000.
If the signed values in the word elements of
.arg m1
and
.arg m2
are larger than 0x7fff, the result elements are clamped to 0x7fff.
.millust begin
        m2                  m1
---------------------   ---------------------
| w3 : w2 | w1 : w0 |   | w3 : w2 | w1 : w0 |
---------------------   ---------------------
     |         |             |         |
     `--------.`---.     .---'.--------'
              |    |     |    |
              V    V     V    V
            ---------------------
            | w3 | w2 | w1 | w0 |
            ---------------------
                    result
.millust end
.desc end
.return begin
The result of packing, with signed saturation, 32-bit signed
double-words into 16-bit signed words is returned.
.return end
.see begin
.im seemmpck
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
__m64   b = { 0x0000567800001234 };
__m64   c = { 0xfffffffe00010101 };
.exmp break
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
.exmp output
m2=fffffffe 00010101 m1=00005678 00001234
mm=fffe 7fff 5678 1234
.exmp end
.class Intel
.system
