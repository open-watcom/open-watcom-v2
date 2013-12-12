.func _m_psllw
.synop begin
#include <mmintrin.h>
__m64 _m_psllw(__m64 *m, __m64 *count);
.synop end
.desc begin
The 16-bit words in
.arg m
are each independently shifted to the left by the scalar shift count
in
.arg count
.ct .li .
The low-order bits of each element are filled with zeros.
The shift count is interpreted as unsigned.
Shift counts greater than 15 yield all zeros.
.desc end
.return begin
Shift left each 16-bit word in
.arg m
by an amount specified in
.arg count
while shifting in zeros.
.return end
.see begin
.im seemmsll &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_WORDS "%4.4x %4.4x %4.4x %4.4x"
#define AS_QWORD "%16.16Lx"
.exmp break
__m64   a;
__m64   b = { 0x3f04800300020001 };
__m64   c = { 0x0000000000000002 };

void main()
  {
    a = _m_psllw( b, c );
    printf( "m1="AS_WORDS"\n"
            "m2="AS_QWORD"\n"
            "mm="AS_WORDS"\n",
        b._16[3], b._16[2], b._16[1], b._16[0],
        c,
        a._16[3], a._16[2], a._16[1], a._16[0] );
  }
.exmp output
m1=3f04 8003 0002 0001
m2=0000000000000002
mm=fc10 000c 0008 0004
.exmp end
.class Intel
.system
