.func _dos_setftime
.synop begin
#include <&doshdr>
unsigned _dos_setftime( int &fd,
                        unsigned date,
                        unsigned time );
.ixfunc2 '&DosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses system call 0x57 to set the date and time that
the file associated with
.arg &fd
was last modified.
The date consists of the year, month and day packed into 16 bits as
follows:
.begnote $compact $setptnt 10
.termhd1 Bits
.termhd2 Meaning
.note bits 0-4
Day (1-31)
.note bits 5-8
Month (1-12)
.note bits 9-15
Year (0-119 representing 1980-2099)
.endnote
.np
The time consists of the hour, minute and seconds/2 packed into 16
bits as follows:
.begnote $compact $setptnt 10
.termhd1 Bits
.termhd2 Meaning
.note bits 0-4
Seconds/2 (0-29)
.note bits 5-10
Minutes (0-59)
.note bits 11-15
Hours (0-23)
.endnote
.desc end
.return begin
The
.id &funcb.
function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_setftime _dos_getftime
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
#include <fcntl.h>
.exmp break
#define YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define MONTH(t)  ((t & 0x01E0) >> 5)
#define DAY(t)    (t & 0x001F)
#define HOUR(t)   ((t & 0xF800) >> 11)
#define MINUTE(t) ((t & 0x07E0) >> 5)
#define SECOND(t) ((t & 0x001F) << 1)
.exmp break
void main( void )
{
    int      &fd;
    unsigned short date, time;
.exmp break
    if( _dos_open( "file", O_RDWR, &amp.&fd ) != 0 ) {
        printf( "Unable to open file\n" );
    } else {
      printf( "Open succeeded\n" );
        _dos_getftime( &fd, &amp.date, &amp.time );
        printf( "The file was last modified on %d/%d/%d",
                MONTH(date), DAY(date), YEAR(date) );
        printf( " at %.2d:%.2d:%.2d\n",
                HOUR(time), MINUTE(time), SECOND(time) );
        /* set the time to 12 noon */
        time = (12 << 11) + (0 << 5) + 0;
        _dos_setftime( &fd, date, time );
        _dos_getftime( &fd, &amp.date, &amp.time );
        printf( "The file was last modified on %d/%d/%d",
                MONTH(date), DAY(date), YEAR(date) );
        printf( " at %.2d:%.2d:%.2d\n",
                HOUR(time), MINUTE(time), SECOND(time) );
        _dos_close( &fd );
    }
}
.exmp output
Open succeeded
The file was last modified on 12/29/1989 at 14:32:46
The file was last modified on 12/29/1989 at 12:00:00
.exmp end
.class DOS
.system
