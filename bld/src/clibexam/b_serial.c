#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short status;

    status = _bios_serialcom( _COM_STATUS, 1, 0 );
    printf( "Serial status: 0x%2.2X\n", status );
  }
