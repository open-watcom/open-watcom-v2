#include <stdio.h>
#include <stdlib.h>

unsigned int mask = 0x0F00;

void main()
  {
    mask = _rotl( mask, 4 );
    printf( "%04X\n", mask );
  }
