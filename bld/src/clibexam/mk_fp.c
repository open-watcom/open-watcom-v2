#include <i86.h>
#include <stdio.h>

void main()
  {
    unsigned short __far *bios_prtr_port_1;

    bios_prtr_port_1 =
            (unsigned short __far *) MK_FP( 0x40, 0x8 );
    printf( "Port address is %x\n", *bios_prtr_port_1 );
  }
