/*
    SCREEN.C - This example shows how to write directly
    to screen memory under the DOS/4GW dos-extender.

    Compile & Link: wcl386 /l=dos4g SCREEN
 */
#include <stdio.h>
#include <dos.h>

/*
    Under DOS/4GW, the first megabyte of physical memory - the
    real memory - is mapped as a shared linear address space.
    This allows your application to access video RAM using its
    linear address.  The DOS segment:offset of B800:0000
    corresponds to a linear address of B8000.
 */
#define SCREEN_AREA 0xb800
#define SCREEN_LIN_ADDR ((SCREEN_AREA) << 4)
#define SCREEN_SIZE 80*25

void main()
{
    char       *ptr;
    int         i;

    /* Set the pointer to the screen's linear address */
    ptr = (char *)SCREEN_LIN_ADDR;
    for( i = 0; i < SCREEN_SIZE - 1; i++ ) {
        *ptr = '*';
        ptr += 2 * sizeof( char );
    }
}
