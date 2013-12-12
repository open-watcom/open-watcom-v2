.func _m_pcmpgtw
.synop begin
#include <mmintrin.h>
__m64 _m_pcmpgtw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
If the respective signed words of
.arg m1
are greater than the respective signed words of
.arg m2
.ct , the respective words of the result are set to all ones, otherwise they
are set to all zeros.
.desc end
.return begin
The result of comparing the 16-bit packed signed words of two 64-bit
multimedia values is returned as a sequence of words (0xffff for
greater than, 0x0000 for not greater than).
.return end
.see begin
.im seemmcmp &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
.exmp break
__m64   a;
__m64   b = { 0x0005000300020001 };
__m64   c = { 0x0004ff8000800001 };

void main()
  {
    a = _m_pcmpgtw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=0005 0003 0002 0001
m2=0004 ff80 0080 0001
mm=ffff ffff 0000 0000
.exmp end
.class Intel
.system
