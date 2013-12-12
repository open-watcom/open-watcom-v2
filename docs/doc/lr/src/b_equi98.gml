.func _bios_equiplist for NEC PC98
#include <bios.h>
unsigned short _bios_equiplist( void );
.ixfunc2 '&BiosFunc' &func
.synop end
.desc begin
The &func function determines what hardware and peripherals are
installed on the machine.
.desc end
.return begin
The &func function returns a set of bits indicating what is currently
installed on the machine. Those bits are defined as follows:
.begnote $compact
.termhd1 Bit
.termhd2 Meaning
.sr ptntelmt = 0
.note bit 0
Reserved.
.note bit 1
If true (1), math co-processor is installed.
.note bit 2
Reserved.
.note bits 3-7
The number of Installed disk drive (00000=0 drives, 00001=1 drive,
etc.).
.note bit 8
If true ( 1 ), a mouse is installed.
.note bits 9-11
The number of installed RS-232C serial port.
.note bit 12
If true ( 1 ), GP-IB is installed.
.note bit 13
If true ( 1 ), a sound board is installed.
.note bit 14-15
The number of installed printers.
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
#include <bios.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>

void main()
  {
    struct diskfree_t drvinfo;
    unsigned drive, drivecount, memory, pstatus;
    union {           /* Access equipment either as:  */
      unsigned u;     /*   unsigned or                */
      struct {        /*   bit fields                 */
        unsigned reserved1:1;   /* Reserved           */
        unsigned coprocessor:1; /* Coprocessor?       */
        unsigned reserved2:1;   /* Reserved           */
        unsigned disks:5;       /* Drives             */
        unsigned mouse:1;       /* Mouse              */
        unsigned comports:3;    /* Serial ports       */
        unsigned gpib:1;        /* GP-IB board        */
        unsigned sound:1;       /* Sound board        */
        unsigned printers:2;    /* Number of printers */
      } bits;
    } equip;
    char    y[] = "YES", n[] = "NO";

    /* Get current drive */
    _dos_getdrive( &drive );
    printf( "Current drive:\t\t\t%c:\n",
            'A' + drive - 1 );

    /* Set drive to current drive
       in order to get number of drives */
    _dos_setdrive( drive, &drivecount );

    _dos_getdiskfree( drive, &drvinfo );
    printf( "Disk space free:\t\t%ld\n",
             ( long )drvinfo.avail_clusters *
             drvinfo.sectors_per_cluster *
             drvinfo.bytes_per_sector );

    /* Get new drive and number of
       logical drives in system */
    _dos_getdrive( &drive );
    printf( "Number of logical drives:\t%d\n",
            drivecount );

    memory = _bios_memsize();
    printf( "Memory:\t\t\t\t%dK\n", memory );

    equip.u = _bios_equiplist();

    printf( "Disk drives:\t\t\t%d\n",
            equip.bits.disks );
    printf( "Coprocessor:\t\t\t%s\n",
            equip.bits.coprocessor ? y : n );
    printf( "Mouse driver:\t\t\t%s\n",
            equip.bits.mouse ? y : n );
    printf( "Serial ports:\t\t\t%d\n",
            equip.bits.comports );
    printf( "GP-IB interface:\t\t%s\n",
            equip.bits.gpib ? y : n );
    printf( "FM-sound:\t\t\t%s\n",
            equip.bits.sound ? y : n );
    printf( "Number of printers:\t\t%d\n",
            equip.bits.printers );

    /* Fail if any error bit is on, or
       if either operation bit is off */
    pstatus = _bios_printer( _PRN_STATUS, 0 );
    if( pstatus & 0x01 )
      pstatus = 0;
    else
      pstatus = 1;
    printf( "Printer available:\t\t%s\n",
            pstatus ? y : n );
  }
.exmp end
.class BIOS
.system
