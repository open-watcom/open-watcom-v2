.func _bios_disk for NEC PC98
#include <bios.h>
unsigned short _bios_disk( unsigned service,
                           struct diskinfo_t *diskinfo );
struct  diskinfo_t {        /* disk parameters     */
        unsigned command;   /* command             */
        unsigned drive;     /* drive number        */
        unsigned head;      /* head number         */
        unsigned cylinder;  /* cylinder number     */
        unsigned data_len;  /* data length         */
        unsigned sector_len;/* sector length       */
        unsigned sector;    /* start sector number */
        unsigned nsectors;  /* number of sectors   */
        void _far *buffer;  /* data buffer         */
        void _far *result;  /* result buffer       */
};
.ixfunc2 '&BiosFunc' &func
.funcend
.desc begin
The &func function makes disk access operations specified by
.arg service
.ct .li .
Please exercise caution when using this function because it accesses
disks directly.
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
.millust begin
_DISK_ALTERNATE    _DISK_BADTRACK      _DISK_DIAGNOSTIC
_DISK_FORMATDRIVE  _DISK_FORMATTRACK   _DISK_INITIALIZE
_DISK_OPMODE       _DISK_RECALIBRATE   _DISK_READ
_DISK_READDDAM     _DISK_READID        _DISK_RETRACT
_DISK_SEEK         _DISK_SENSE         _DISK_VERIFY
_DISK_WRITE        _DISK_WRITEDDAM
.millust end
.np
In the following description, <sector> etc. means the field of the structure
.kw diskinfo_t
.ct .li .
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _DISK_ALTERNATE
It sets alternative track. The alternative track address is stored in
the area pointed to by <buffer>. It can be used when _CMD_HD is in
<command>. If one of the other symbol constants is in <command>, it is
an error, but it does not return the value which shows errors.
.term _DISK_BADTRACK
It formats bad tracks. Before this, you should set alternative tracks
by _DISK_ALTERNATE. It can be used when _CMD_HD is in <command>.
.term _DISK_DIAGNOSTIC
It reads data into <buffer> for diagnostis. It can be used when
_CMD_2HD or _CMD_2DD is in <command>. The result status is stored in
the area pointed to by <result>.
.term _DISK_FORMATDRIVE
It formats a disk. Interleave facter is set in <data_len>. it can be
used when _CMD_2D or _CMD_HD is in <command>.
.term _DISK_FORMATTRACK
It formats one track. The sector ID information written in the track
is set in the area pointed to by <buffer>. It can be used
when one of _CMD_2HD, _CMD_2DD or _CMD_HD is in <command>.
.term _DISK_INITIALIZE
It initializes <drive>. It does not use the field <drive> and
<command> only. It can be used when one of _CMD_2HD, _CMD_2DD, _CMD_2D
or _CMD_HD is in <command>. It is different from the case the
other symbol constant is specified, it does not return the status
information when it is error.
.term _DISK_OPMODE
It specifies the action when 640 K bytes of floppy disk is accessed
under 1 mega bytes interface mode. It can be used when _CMD_2HD,
_CMD_2DD or _CMD_2D is in <command>.
.term _DISK_READ
It reads data from the <sector> of the <drive> into <buffer> by
<nsectors> sectors. It can be used when _CMD_2HD, _CMD_2DD, _CMD_2D,
_CMD_HD is in <command>. The result status information is in the area
pointed by <result>.
.term _DISK_READDDAM
It reads Deleted Data Address Marked data into <buffer>. It can be
used when _CMD_2HD or _CMD_2DD is in <command>.
The result status information is in the area pointed to by <result>.
.term _DISK_READID
It reads ID information into <result>. It can be used when _CMD_2HD or
_CMD_2DD is in <command>.
.term _DISK_RECALIBRATE
It seeks cylinder 0 of <drive>. It can be used when _CMD_2HD, _CMD_2DD
or _CMD_HD is in <command>.
.term _DISK_RETRACT
It retracts the head of disk. It canbe used when _CMD_HD is in
<command>.
.term _DISK_SEEK
It seeks the disk. It can be used when _CMD_2HD or _CMD_2DD is in
<command>. The result status informaion is stored in the area pointed
to by <result>.
.term _DISK_SENSE
It checks the status of <drive>. It uses the fields <drive> and
<command> only. It can be used when _CMD_2HD, _CMD_2DD, _CMD_2D or
_CMD_HD is in <command>.
.term _DISK_VERIFY
It reads data from <sector> of <drive> by <nsectors> sectors. It does
not store the data into memory. It can be used when _CMD_2HD,
_CMD_2DD, _CMD_2D or _CMD_HD is in <command>. The result status
information is stored in the area pointed to by <result>.
.term _DISK_WRITE
It writes data stored in the area pointed to by <buffer> to <sector>
of <drive> by <nsectors> sectors. It can be used when CMD_2HD,
_CMD_2DD, _CMD_2D or _CMD_HD is in <command> The result status
information is stored in the area pointed to by <result>.
.term _DISK_WRITEDDAM
It writes Deleted Data Address Marked data. It can be used when
_CMD_2HD or _CMD_2DD is in <command>. The result status information is
stored in the area pointed to by <result>.
.endterm
.desc end
.return begin
If the &func function finishes normally, it returns zero in the upper
byte and the status information in the lower byte. If an error occurs,
the upper byte is not zero and the lower byte is the status
information.
When
.kw _DISK_INITIALIZE
is specified in
.arg service
and an error occurs, the lower byte is zero.
When
.kw _DISK_ALTERNATE
.ct , is specified in
.arg service
.ct , the error value is not returned.
.np
The contents of status information are different depending on the
.arg service
argument.
Also, the contents of the
.mono result
field may or may not be set depending on the value of the
.arg service
argument.
The meaning of the status or result information is described below.
The possible values are:
.begnote $compact
.termhd1 Value
.termhd2 Status information or result status information
.sr ptntelmt = 0
.note 0x00
Normal end/Ready status
.note 0x10
Control mark/Inhibit to write
.note 0x20
DMA boundary
.note 0x30
End of cylinder
.note 0x40
Check device
.note 0x50
Overrun
.note 0x60
Not ready
.note 0x70
Write Disabled
.note 0x80
error
.note 0x90
Time-out
.note 0xA0
Data error (Control information)
.note 0xB0
Data error (Data)
.note 0xC0
No data
.note 0xD0
Bad Cylinder
.note 0xE0
Cannot find address mark (Control information)
.note 0xF0
Cannot find address mark (Data)
.sk 1 c
.note 0x01
Double side diskette is set
.sk 1 c
.note 0x08
Normal data
.note 0x78
Invalid disk address
.note 0x88
Direct access to alternative track
.note 0xB8
Data error
.note 0xC8
Seek error
.note 0xD8
Cannot read alternative track
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
/*
 * This program illustrates low-level disk access
 */
#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <stdlib.h>

char _far diskbuf[ 2048 ], result[ 16 ];

void main( int argc, char *argv[] )
  {
    unsigned status = 0, i;
    struct diskinfo_t di;
    unsigned char _far *p, linebuf[ 17 ];

    if( argc != 5 ) {
      printf( "SYNTAX: DISK <drivenumber> "
              "<head> <track> <sector>" );
      exit( 1 );
    }
    di.command    = _CMD_SEEK | _CMD_MF | _CMD_MT
                  | _CMD_RETRY | _CMD_2HD;
    di.drive      = atoi( argv[ 1 ] );
    di.head       = atoi( argv[ 2 ] );
    di.cylinder   = atoi( argv[ 3 ] );
    di.sector     = atoi( argv[ 4 ] );
    di.data_len   = 2048;
    di.sector_len = 3;
    di.buffer     = diskbuf;
    di.result     = result;

    /* Read disk */
    status = _bios_disk( _DISK_READ, &di );

    /* Display one sector */
    if( status &= 0x00ff )
      printf( "Error: 0x%.2x\n", status );
    else {
      for( p = diskbuf, i = 0;
           p < ( diskbuf + ( 128 << di.sector_len ) );
           p++ ) {
        linebuf[ i++ ] = ( *p >= 0x20 && *p <= 0x7f )
                         ? *p : '.';
        printf( "%.2x ", *p );
        if( i == 16 ) {
          linebuf[ i ] = '\0';
          printf( " %16s\n", linebuf );
          i = 0;
        }
      }
    }
    exit( 1 );
  }




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
