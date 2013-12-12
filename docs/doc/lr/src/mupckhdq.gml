.func _m_punpckhdq
.synop begin
#include <mmintrin.h>
__m64 _m_punpckhdq(__m64 *m1, __m64 *m2);
.synop end
.desc begin
The &func function performs an interleaved unpack of the
high-order data elements of
.arg m1
and
.arg m2
.ct .li .
It ignores the low-order double-words.
When unpacking from a memory operand, the full 64-bit operand
is accessed from memory but only the high-order 32 bits are utilized.
.millust begin
            m2                         m1
-------------------------  -------------------------
|     d1    |     d0    |  |     d1    |     d0    |
-------------------------  -------------------------
      |                          |
      V                          V
      d1                         d0

              -------------------------
              |     d1    |     d0    |
              -------------------------
                    result
.millust end
.desc end
.return begin
The result of the interleaved unpacking of the high-order double-words
of two multimedia values is returned.
.return end
.see begin
.im seemmupk &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

#define AS_DWORDS "%8.8lx %8.8lx"
.exmp break
__m64   a;
__m64   b = { 0x0004000300020001 };
__m64   c = { 0xff7fff800080007f };
.exmp break
void main()
  {
    a = _m_punpckhdq( b, c );
    printf( "m2="AS_DWORDS" "
            "m1="AS_DWORDS"\n"
            "mm="AS_DWORDS"\n",
        c._32[1], c._32[0],
        b._32[1], b._32[0],
        a._32[1], a._32[0] );
  }
.exmp output
m2=ff7fff80 0080007f m1=00040003 00020001
mm=ff7fff80 00040003
.exmp end
.class Intel
.system
