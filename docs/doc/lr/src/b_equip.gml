.func _bios_equiplist
.synop begin
#include <bios.h>
unsigned short _bios_equiplist( void );
.ixfunc2 '&BiosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses INT 0x11 to determine what hardware and
peripherals are installed on the machine.
.desc end
.return begin
The
.id &funcb.
function returns a set of bits indicating what is currently
installed on the machine. Those bits are defined as follows:
.begnote $compact
.termhd1 Bit
.termhd2 Meaning
.setptnt 0 10
.sr ptntelmt = 0
.note bit 0
Set to 1 if system boots from disk
.note bit 1
Set to 1 if a math coprocessor is installed
.note bits 2-3
Indicates motherboard RAM size
.note bits 4-5
Initial video mode
.note bits 6-7
Number of diskette drives
.note bit 8
Set to 1 if machine does not have DMA
.note bits 9-11
Number of serial ports
.note bit 12
Set to 1 if a game port is attached
.note bit 13
Set to 1 if a serial printer is attached
.note bits 14-15
Number of parallel printers installed
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short equipment;
.exmp break
    equipment = _bios_equiplist();
    printf( "Equipment flags = 0x%4.4X\n", equipment );
  }
.exmp end
.class BIOS
.system
