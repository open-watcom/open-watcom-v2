#include <stdio.h>
#include <stdlib.h>

unsigned long mask = 0x12345678;

void main()
  {
    mask = _lrotr( mask, 4 );
    printf( "%08lX\n", mask );
  }
