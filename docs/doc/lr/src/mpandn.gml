.func _m_pandn
.synop begin
#include <mmintrin.h>
__m64 _m_pandn(__m64 *m1, __m64 *m2);
.synop end
.desc begin
A bit-wise logical AND is performed on the logical inversion of
64-bit multimedia operand
.arg m1
and 64-bit multimedia operand
.arg m2
and the result is stored in memory.
.desc end
.return begin
The bit-wise logical AND of an inverted 64-bit value and a
non-inverted value is returned.
.return end
.see begin
.im seemmbit
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
    a = _m_pandn( b, c );
    printf( "m1="AS_QWORD"\n"
            "m2="AS_QWORD"\n"
            "mm="AS_QWORD"\n",
            b, c, a );
  }
.exmp output
m1=0123456789abcdef
m2=fedcba9876543210
mm=fedcba9876543210
.exmp end
.class Intel
.system
