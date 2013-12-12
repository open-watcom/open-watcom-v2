.func _m_pxor
.synop begin
#include <mmintrin.h>
__m64 _m_pxor(__m64 *m1, __m64 *m2);
.synop end
.desc begin
A bit-wise logical XOR is performed between 64-bit multimedia operands
.arg m1
and
.arg m2
and the result is stored in memory.
.desc end
.return begin
The bit-wise logical exclusive OR of two 64-bit values is returned.
.return end
.see begin
.im seemmbit &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_QWORD "%16.16Lx"
.exmp break
__m64   a;
__m64   b = { 0x0123456789abcdef };
__m64   c = { 0xfedcba9876543210 };

void main()
  {
    a = _m_pxor( b, c );
    printf( "m1="AS_QWORD"\n"
            "m2="AS_QWORD"\n"
            "mm="AS_QWORD"\n",
            b, c, a );
  }
.exmp output
m1=0123456789abcdef
m2=fedcba9876543210
mm=ffffffffffffffff
.exmp end
.class Intel
.system
