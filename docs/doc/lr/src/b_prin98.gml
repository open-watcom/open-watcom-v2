.func _bios_printer for NEC PC98
#include <bios.h>
unsigned short _bios_printer( unsigned service,
                              unsigned char *databyte );
.ixfunc2 '&BiosFunc' &func
.funcend
.desc begin
The &func function performs printer output services to the printer.
The values for service are:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _PRN_INIT
It initializes the status area and the controller.
Bit 0 of the return value indicates the following:.
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 1
Enabled to output data
.note 0
Disabled to output data
.endnote
.term _PRN_WRITE
It outputs one byte of data from the area pointed to by the argument
.arg data
.ct .li .
Bit 0 of the return value indicates the following:.
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 1
Date output complete, enabled to output more data.
.note 0
Data is not output.
.endnote
.np
Bit 1 of the return value indicates the following:.
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 1
Time-out, the data is not output.
.note 0
Data output complete.
.endnote
.term _PRN_STRING
It outputs the data in the area pointed to by the argument
.arg data
till the first null character. It returns zero if no error occurs and
the number of characters which are not output if an error occurs.
.term _PRN_STATUS
It gets the status information of the printer.
Bit 0 of the return value indicates the following:.
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 1
Enabled for data output
.note 0
Disabled from data output
.endnote
.endterm
.desc end
.return begin
The &func function returns a printer status byte described above.
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short status;
.exmp break
    status = _bios_printer( _PRN_STATUS, 0 );
    printf( "Printer status: 0x%2.2X\n", status );
  }
.exmp end
.class BIOS
.system
