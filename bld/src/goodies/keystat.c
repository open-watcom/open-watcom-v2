/*
    KEYSTAT.C - This example shows how to get the keyboard
    status under DOS/4GW by looking at the ROM BIOS
    keyboard status byte in low memory.

    Compile & Link: wcl386 /l=dos4g keystat
 */
#include <stdio.h>
#include <dos.h>

/*
    Under DOS, the keyboard status byte has a segmented
    address of 0x0040:0x0017.  This corresponds to a
    linear address of 0x417.
 */
#define LOW_AREA 0x417

void main()
{
    /* Only need a near pointer in the flat model */
    char *ptr;

    /* Set pointer to linear address of the first
       status byte */
    ptr = (char *)LOW_AREA;

    /* Caps lock state is in bit 6 */
    if( *ptr & 0x40 ) {
        puts( "Caps Lock on" );
    }
    /* Num lock state is in bit 5 */
    if( *ptr & 0x20 ) {
        puts( "Num Lock on" );
    }
    /* Scroll lock state is in bit 4 */
    if( *ptr & 0x10 ) {
        puts( "Scroll Lock on" );
    }
}
