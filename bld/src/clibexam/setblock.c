#include <stdio.h>
#include <dos.h>

void main( void )
{
    unsigned segment;
    unsigned maxsize;

    /* Try to allocate 100 paragraphs, then free them */
    if( _dos_allocmem( 100, &segment ) != 0 ) {
        printf( "_dos_allocmem failed\n" );
        printf( "Only %u paragraphs available\n", segment);
    } else {
        printf( "_dos_allocmem succeeded\n" );
        /* Try to increase it to 200 paragraphs */
        if( _dos_setblock( 200, segment, &maxsize ) != 0 ) {
            printf( "Unable to increase the size\n" );
        }
        if( _dos_freemem( segment ) != 0 ) {
            printf( "_dos_freemem failed\n" );
        }
    }
}
