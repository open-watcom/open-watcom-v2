.func _m_paddusw
#include <mmintrin.h>
__m64 _m_paddusw(__m64 *m1, __m64 *m2);
.funcend
.desc begin
The unsigned 16-bit words of
.arg m2
are added to the respective unsigned 16-bit words of
.arg m1
and the result is stored in memory.
Saturation occurs when a result exceeds the range of an unsigned word.
In the case where a result is a word larger than 0xffff (overflow), it
is clamped to 0xffff.
.desc end
.return begin
The result of adding the packed unsigned words, with saturation, of
two 64-bit multimedia values is returned.
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
__m64   b = { 0x8aacceef02244668 };
__m64   c = { 0x76543211fedcba98 };

void main()
  {
    a = _m_paddusw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=8aac ceef 0224 4668
m2=7654 3211 fedc ba98
mm=ffff ffff ffff ffff
.exmp end
.class Intel
.system
