.func _m_pmulhw
#include <mmintrin.h>
__m64 _m_pmulhw(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The signed 16-bit words of
.arg m1
are multiplied with the respective signed 16-bit words of
.arg m2
.ct .li .
The high-order 16-bits of each result are placed in the respective
elements of the result.
.desc end
.return begin
The packed 16-bit words in
.arg m1
are multiplied with the packed 16-bit words in
.arg m2
and the high-order 16-bits of the results are returned.
.return end
.see begin
.im seemmmul &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
.exmp break
__m64   a;
__m64   b = { 0x4000006000123456 };
__m64   c = { 0x0008000210000020 };

void main()
  {
    a = _m_pmulhw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=4000 0060 0012 3456
m2=0008 0002 1000 0020
mm=0002 0000 0001 0006
.exmp end
.class Intel
.system
