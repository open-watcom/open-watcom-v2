.func _m_paddsw
.synop begin
#include <mmintrin.h>
__m64 _m_paddsw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The signed 16-bit words of
.arg m2
are added to the respective signed 16-bit words of
.arg m1
and the result is stored in memory.
Saturation occurs when a result exceeds the range of a signed
word.
In the case where a result is a word larger than 0x7fff (overflow), it
is clamped to 0x7fff.
In the case where a result is a word smaller than 0x8000 (underflow),
it is clamped to 0x8000.
.desc end
.return begin
The result of adding the packed signed words, with saturation, of two
64-bit multimedia values is returned.
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
    a = _m_paddsw( b, c );
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
mm=0100 0100 0100 0100
.exmp end
.class Intel
.system
