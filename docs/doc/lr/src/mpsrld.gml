.func _m_psrld
#include <mmintrin.h>
__m64 _m_psrld(__m64 *m, __m64 *count);
.synop end
.desc begin
The 32-bit double-words in
.arg m
are each independently shifted to the right by the scalar shift count
in
.arg count
.ct .li .
The high-order bits of each element are filled with zeros.
The shift count is interpreted as unsigned.
Shift counts greater than 31 yield all zeros.
.desc end
.return begin
Shift right each 32-bit double-word in
.arg m
by an amount specified in
.arg count
while shifting in zeros.
.return end
.see begin
.im seemmsrl &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_DWORDS "%8.8lx %8.8lx"
#define AS_QWORD "%16.16Lx"
.exmp break
__m64   a;
__m64   b = { 0x3f04800300020001 };
__m64   c = { 0x0000000000000002 };

void main()
  {
    a = _m_psrld( b, c );
    printf( "m1="AS_DWORDS"\n"
            "m2="AS_QWORD"\n"
            "mm="AS_DWORDS"\n",
        b._32[1], b._32[0],
        c,
        a._32[1], a._32[0] );
  }
.exmp output
m1=3f048003 00020001
m2=0000000000000002
mm=0fc12000 00008000
.exmp end
.class Intel
.system
