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
