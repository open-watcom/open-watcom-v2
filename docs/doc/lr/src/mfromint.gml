.func _m_from_int
#include <mmintrin.h>
__m64 _m_from_int(int i);
.funcend
.desc begin
The &func function forms a 64-bit MM value from an unsigned 32-bit
integer value.
.desc end
.return begin
The 64-bit result of loading MM0 with an unsigned 32-bit integer value
is returned.
.return end
.see begin
.im seemmfun &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

__m64   a;

int     k = 0xF1F2F3F4;

void main()
  {
    a = _m_from_int( k );
    printf( "int=%8.8lx m=%8.8lx%8.8lx\n",
        k, a._32[1], a._32[0] );

  }
.exmp output
int=f1f2f3f4 m=00000000f1f2f3f4
.exmp end
.class Intel
.system
