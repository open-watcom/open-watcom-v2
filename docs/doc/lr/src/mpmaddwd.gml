.func _m_pmaddwd
#include <mmintrin.h>
__m64 _m_pmaddwd(__m64 *m1, __m64 *m2);
.funcend
.desc begin
The signed 16-bit words of
.arg m1
are multiplied with the respective signed 16-bit words
of
.arg m2
.ct .li .
The 32-bit intermediate results are summed by pairs producing
two 32-bit integers.
.millust begin
MM[63-32] = M1[63-48] x M2[63-48]
          + M1[47-32] x M2[47-32]
MM[31-0]  = M1[31-16] x M2[31-16]
          + M1[15-0]  x M2[15-0]
.millust end
.np
In cases which overflow, the results are truncated.
These two integers are packed into their respective elements
of the result.
.desc end
.return begin
The result of multiplying the packed signed 16-bit words of two 64-bit
multimedia values and adding the 32-bit results pairwise is returned
as packed double-words.
.return end
.see begin
.im seemmmul &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
#define AS_DWORDS "%8.8lx %8.8lx"
.exmp break
__m64   a;
__m64   b = { 0x0000006000123456 };
__m64   c = { 0x0000000200010020 };

void main()
  {
    a = _m_pmaddwd( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_WORDS"\n"
            "mm="AS_DWORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c._16[3], c._16[2], c._16[1], c._16[0],
        a._32[1], a._32[0] );
  }
.exmp output
m1=0000 0060 0012 3456
m2=0000 0002 0001 0020
mm=000000c0 00068ad2
.exmp end
.class Intel
.system
