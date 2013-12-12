.func _m_psradi
#include <mmintrin.h>
__m64 _m_psradi(__m64 *m, int count);
.synop end
.desc begin
The 32-bit signed double-words in
.arg m
are each independently shifted to the right by the scalar shift count
in
.arg count
.ct .li .
The high-order bits of each element are filled with the initial value
of the sign bit of each element.
The shift count is interpreted as unsigned.
Shift counts greater than 31 yield all ones or zeros depending on the
initial value of the sign bit.
.desc end
.return begin
Shift right each 32-bit double-word in
.arg m
by an amount specified in
.arg count
while shifting in sign bits.
.return end
.see begin
.im seemmsra &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_DWORDS "%8.8lx %8.8lx"
.exmp break
__m64   a;
__m64   b = { 0x3f04800300020001 };

void main()
  {
    a = _m_psradi( b, 2 );
    printf( "m ="AS_DWORDS"\n"
            "mm="AS_DWORDS"\n",
        b._32[1], b._32[0],
        a._32[1], a._32[0] );
  }
.exmp output
m =3f048003 00020001
mm=0fc12000 00008000
.exmp end
.class Intel
.system
