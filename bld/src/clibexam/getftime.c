#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

#define YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define MONTH(t)  ((t & 0x01E0) >> 5)
#define DAY(t)    (t & 0x001F)
#define HOUR(t)   ((t & 0xF800) >> 11)
#define MINUTE(t) ((t & 0x07E0) >> 5)
#define SECOND(t) ((t & 0x001F) << 1)

void main( void )
{
    int      handle;
    unsigned date, time;

    if( _dos_open( "file", O_RDONLY, &handle ) != 0 ) {
        printf( "Unable to open file\n" );
    } else {
        printf( "Open succeeded\n" );
        _dos_getftime( handle, &date, &time );
        printf( "The file was last modified on %d/%d/%d",
                MONTH(date), DAY(date), YEAR(date) );
        printf( " at %.2d:%.2d:%.2d\n",
                HOUR(time), MINUTE(time), SECOND(time) );
        _dos_close( handle );
    }
}
