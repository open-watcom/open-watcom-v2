/*
    KEYSTAPL.C - This example shows how to get the keyboard
    status under 386|DOS-Extender by looking at the ROM
    BIOS keyboard status byte in low memory.

    Compile & Link: wcl386 /l=pharlap keystapl
 */
#include <stdio.h>
#include <dos.h>

/*
    Under DOS, the keyboard status byte has a segmented
    address of 0x0040:0x0017.  This corresponds to a
    linear address of 0x417.
 */

void main()
{
    /* We require a far pointer to use selector
       for 1st megabyte */
    char far *ptr;

    /* Set pointer to segmented address of the first
       status byte */
    ptr = MK_FP( 0x34, 0x417 );

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
