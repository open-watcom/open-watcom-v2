#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short status;

    status = _bios_printer( _PRINTER_STATUS, 1, 0 );
    printf( "Printer status: 0x%2.2X\n", status );
  }
