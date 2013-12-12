.func _m_psubb
.synop begin
#include <mmintrin.h>
__m64 _m_psubb(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The signed or unsigned 8-bit bytes of
.arg m2
are subtracted from the respective signed or unsigned 8-bit bytes of
.arg m1
and the result is stored in memory.
If any result element does not fit into 8 bits (underflow or
overflow), the lower 8 bits of the result elements are stored (i.e.,
truncation takes place).
.desc end
.return begin
The result of subtracting the packed bytes of one 64-bit multimedia
value from another is returned.
.return end
.see begin
.im seemmsub &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_BYTES "%2.2x %2.2x %2.2x %2.2x " \
                 "%2.2x %2.2x %2.2x %2.2x"
.exmp break
__m64   a;
__m64   b = { 0x0123456789abcdef };
__m64   c = { 0xfedcba9876543210 };

void main()
  {
    a = _m_psubb( b, c );
    printf( "m1="AS_BYTES"\n"
            "m2="AS_BYTES"\n"
            "mm="AS_BYTES"\n",
        b._8[7], b._8[6], b._8[5], b._8[4],
        b._8[3], b._8[2], b._8[1], b._8[0],
        c._8[7], c._8[6], c._8[5], c._8[4],
        c._8[3], c._8[2], c._8[1], c._8[0],
        a._8[7], a._8[6], a._8[5], a._8[4],
        a._8[3], a._8[2], a._8[1], a._8[0] );
  }
.exmp output
m1=01 23 45 67 89 ab cd ef
m2=fe dc ba 98 76 54 32 10
mm=03 47 8b cf 13 57 9b df
.exmp end
.class Intel
.system
