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
