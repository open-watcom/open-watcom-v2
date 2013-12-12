.func _m_psrlw
#include <mmintrin.h>
__m64 _m_psrlw(__m64 *m, __m64 *count);
.synop end
.desc begin
The 16-bit words in
.arg m
are each independently shifted to the right by the scalar shift count
in
.arg count
.ct .li .
The high-order bits of each element are filled with zeros.
The shift count is interpreted as unsigned.
Shift counts greater than 15 yield all zeros.
.desc end
.return begin
Shift right each 16-bit word in
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

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
#define AS_QWORD "%16.16Lx"
.exmp break
__m64   a;
__m64   b = { 0x3f04800300040001 };
__m64   c = { 0x0000000000000002 };

void main()
  {
    a = _m_psrlw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_QWORD"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c,
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=3f04 8003 0004 0001
m2=0000000000000002
mm=0fc1 2000 0001 0000
.exmp end
.class Intel
.system
