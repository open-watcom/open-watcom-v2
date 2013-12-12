.func _m_pcmpeqw
.synop begin
#include <mmintrin.h>
__m64 _m_pcmpeqw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
If the respective words of
.arg m1
are equal to the respective words of
.arg m2
.ct , the respective words of the result are set to all ones, otherwise they
are set to all zeros.
.desc end
.return begin
The result of comparing the packed words of two 64-bit multimedia
values is returned as a sequence of words (0xffff for equal, 0x0000
for not equal).
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
__m64   b = { 0x0004000300020001 };
__m64   c = { 0x0004ff8000800001 };

void main()
  {
    a = _m_pcmpeqw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=0004 0003 0002 0001
m2=0004 ff80 0080 0001
mm=ffff 0000 0000 ffff
.exmp end
.class Intel
.system
