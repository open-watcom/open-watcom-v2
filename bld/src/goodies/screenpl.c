/*
    SCREENPL.C - This example shows how to write directly
    to screen memory under the Phar Lap DOS extender.

    Compile & Link: wcl386 /l=pharlap SCREENPL
 */
#include <stdio.h>
#include <dos.h>

/*
    Phar Lap allows access to screen memory through a
    special selector.  Refer to "Hardware Access" in
    Phar Lap's documentation for details.
 */
#define PL_SCREEN_SELECTOR 0x1c
#define SCREEN_SIZE 80*25

void main()
{
    /* Need a far pointer to use the screen selector */
    char far   *ptr;
    int         i;

    /* Make a far pointer to screen memory */
    ptr = MK_FP( PL_SCREEN_SELECTOR, 0 );
    for( i = 0; i < SCREEN_SIZE - 1; i++ ) {
        *ptr = '*';
        ptr += 2 * sizeof( char );
    }
}
