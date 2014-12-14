.func _m_psrawi
.synop begin
#include <mmintrin.h>
__m64 _m_psrawi(__m64 *m, int count);
.synop end
.desc begin
The 16-bit signed words in
.arg m
are each independently shifted to the right by the scalar shift count
in
.arg count
.ct .li .
The high-order bits of each element are filled with the initial value
of the sign bit of each element.
The shift count is interpreted as unsigned.
Shift counts greater than 15 yield all ones or zeros depending on the
initial value of the sign bit.
.desc end
.return begin
Shift right each 16-bit word in
.arg m
by an amount specified in
.arg count
while shifting in sign bits.
.return end
.see begin
.im seemmsra
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
.exmp break
__m64   a;
__m64   b = { 0x3f04800300040001 };

void main()
  {
    a = _m_psrawi( b, 2 );
    printf( "m ="AS_WORDS"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m =3f04 8003 0004 0001
mm=0fc1 e000 0001 0000
.exmp end
.class Intel
.system
