#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short equipment;

    equipment = _bios_equiplist();
    printf( "Equipment flags = 0x%4.4X\n", equipment );
  }
