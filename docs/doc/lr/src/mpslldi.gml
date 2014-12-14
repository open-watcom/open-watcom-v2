.func _m_pslldi
.synop begin
#include <mmintrin.h>
__m64 _m_pslldi(__m64 *m, int count);
.synop end
.desc begin
The 32-bit double-words in
.arg m
are each independently shifted to the left by the scalar shift count
in
.arg count
.ct .li .
The low-order bits of each element are filled with zeros.
The shift count is interpreted as unsigned.
Shift counts greater than 31 yield all zeros.
.desc end
.return begin
Shift left each 32-bit double-word in
.arg m
by an amount specified in
.arg count
while shifting in zeros.
.return end
.see begin
.im seemmsll
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
    a = _m_pslldi( b, 2 );
    printf( "m ="AS_DWORDS"\n"
            "mm="AS_DWORDS"\n",
        b._32[1], b._32[0],
        a._32[1], a._32[0] );
  }
.exmp output
m =3f048003 00020001
mm=fc12000c 00080004
.exmp end
.class Intel
.system
