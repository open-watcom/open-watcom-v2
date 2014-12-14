.func _m_pcmpeqd
.synop begin
#include <mmintrin.h>
__m64 _m_pcmpeqd(__m64 *m1, __m64 *m2);
.synop end
.desc begin
If the respective double-words of
.arg m1
are equal to the respective double-words of
.arg m2
.ct , the respective double-words of the result are set to all ones,
otherwise they are set to all zeros.
.desc end
.return begin
The result of comparing the 32-bit packed double-words of two 64-bit
multimedia values is returned as a sequence of double-words
(0xffffffff for equal, 0x00000000 for not equal).
.return end
.see begin
.im seemmcmp
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_DWORDS "%8.8lx %8.8lx"
.exmp break
__m64   a;
__m64   b = { 0x0004000300020001 };
__m64   c = { 0x000400030002007f };

void main()
  {
    a = _m_pcmpeqd( b, c );
    printf( "m1="AS_DWORDS"\n"
            "m2="AS_DWORDS"\n"
            "mm="AS_DWORDS"\n",
        b._32[1], b._32[0],
        c._32[1], c._32[0],
        a._32[1], a._32[0] );
  }
.exmp output
m1=00040003 00020001
m2=00040003 0002007f
mm=ffffffff 00000000
.exmp end
.class Intel
.system
