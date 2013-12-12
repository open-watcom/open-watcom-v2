.func _m_paddw
#include <mmintrin.h>
__m64 _m_paddw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The signed or unsigned 16-bit words of
.arg m2
are added to the respective signed or unsigned 16-bit words of
.arg m1
and the result is stored in memory.
If any result element does not fit into 16 bits (overflow), the lower
16 bits of the result elements are stored (i.e., truncation takes
place).
.desc end
.return begin
The result of adding the packed words of two 64-bit multimedia values
is returned.
.return end
.see begin
.im seemmadd &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
.exmp break
__m64   a;
__m64   b = { 0x0123456789abcdef };
__m64   c = { 0xfedcba9876543210 };

void main()
  {
    a = _m_paddw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=0123 4567 89ab cdef
m2=fedc ba98 7654 3210
mm=ffff ffff ffff ffff
.exmp end
.class Intel
.system
