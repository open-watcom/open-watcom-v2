.func _m_to_int
#include <mmintrin.h>
int   _m_to_int(__m64 *__m);
.funcend
.desc begin
The &func function returns the low-order 32 bits of a multimedia value.
.desc end
.return begin
The low-order 32 bits of a multimedia value are fetched and returned
as the result.
.return end
.see begin
.im seemmfun &function.
.see end
.exmp begin
#include <stdio.h>
#include <mmintrin.h>

__m64   b = { 0x0123456789abcdef };

int     j;

void main()
  {
    j = _m_to_int( b );
    printf( "m=%16.16Lx int=%8.8lx\n",
            b, j );
  }
.exmp output
m=0123456789abcdef int=89abcdef
.exmp end
.class Intel
.system
