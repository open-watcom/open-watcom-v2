#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short memsize;

    memsize = _bios_memsize();
    printf( "The total amount of memory is: %dK\n",
               memsize );
  }
