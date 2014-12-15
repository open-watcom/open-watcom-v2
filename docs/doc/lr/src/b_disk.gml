.func _bios_disk
.synop begin
#include <bios.h>
unsigned short _bios_disk( unsigned service,
                           struct diskinfo_t *diskinfo );
struct  diskinfo_t {        /* disk parameters   */
        unsigned drive;     /* drive number      */
        unsigned head;      /* head number       */
        unsigned track;     /* track number      */
        unsigned sector;    /* sector number     */
        unsigned nsectors;  /* number of sectors */
        void __far *buffer; /* buffer address    */
};
.ixfunc2 '&BiosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses INT 0x13 to provide access to the BIOS
disk functions.
Information for the desired
.arg service
is passed the
.kw diskinfo_t
structure pointed to by
.arg diskinfo
.ct .li .
The value for
.arg service
can be one of the following values:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _DISK_RESET
Forces the disk controller to do a reset on the disk.
This request does not use the
.arg diskinfo
argument.
.term _DISK_STATUS
Obtains the status of the last disk operation.
.term _DISK_READ
Reads the specified number of sectors from the disk.
This request uses all of the information passed in the
.arg diskinfo
structure.
.term _DISK_WRITE
Writes the specified amount of data to the disk.
This request uses all of the information passed in the
.arg diskinfo
structure.
.term _DISK_VERIFY
Checks the disk to be sure the specified sectors exist and can be read.
A CRC (cyclic redundancy check) test is performed.
This request uses all of the information passed in the
.arg diskinfo
structure except for the
.arg buffer
field.
.term _DISK_FORMAT
Formats the specified track on the disk.
The
.arg head
and
.arg track
fields indicate the track to be formatted.
Only one track can be formatted per call.
The
.arg buffer
field points to a set of sector markers, whose format depends on the
type of disk drive.
This service has no return value.
.endterm
.np
This function is not supported by DOS/4GW (you must use the Simulate
Real-Mode Interrupt DPMI call).
.desc end
.return begin
The
.id &funcb.
function returns status information in the high-order byte when
.arg service
is _DISK_STATUS, _DISK_READ, _DISK_WRITE, or _DISK_VERIFY.
The possible values are:
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.sr ptntelmt = 0
.note 0x00
Operation successful
.note 0x01
Bad command
.note 0x02
Address mark not found
.note 0x03
Attempt to write to write-protected disk
.note 0x04
Sector not found
.note 0x05
Reset failed
.note 0x06
Disk changed since last operation
.note 0x07
Drive parameter activity failed
.note 0x08
DMA overrun
.note 0x09
Attempt to DMA across 64K boundary
.note 0x0A
Bad sector detected
.note 0x0B
Bad track detected
.note 0x0C
Unsupported track
.note 0x10
Data read (CRC/ECC) error
.note 0x11
CRC/ECC corrected data error
.note 0x20
Controller failure
.note 0x40
Seek operation failed
.note 0x80
Disk timed out or failed to respond
.note 0xAA
Drive not ready
.note 0xBB
Undefined error occurred
.note 0xCC
Write fault occurred
.note 0xE0
Status error
.note 0xFF
Sense operation failed
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    struct diskinfo_t di;
    unsigned short status;
.exmp break
    di.drive = di.head = di.track = di.sector = 0;
    di.nsectors = 1;
    di.buffer = NULL;
    status = _bios_disk( _DISK_VERIFY, &di );
    printf( "Status = 0x%4.4X\n", status );
  }
.exmp end
.class BIOS
.system
