#include <stdio.h>
#include <dos.h>

int delays = 0;
volatile int clock_ticks;
void (__interrupt __far *prev_int_1c)( void );
#define BLIP_COUNT  (5*18)  /* 5 seconds */

void __interrupt __far timer_rtn( void )
{
    ++clock_ticks;
    _chain_intr( prev_int_1c );
}

int compile_a_line( void )
{
    if( delays > 15 ) return( 0 );
    delay( 1000 );  /* delay for 1 second */
    printf( "Delayed for 1 second\n" );
    delays++;
    return( 1 );
}

void main( void )
{
    prev_int_1c = _dos_getvect( 0x1c );
    _dos_setvect( 0x1c, timer_rtn );
    while( compile_a_line() ) {
        if( clock_ticks >= BLIP_COUNT ) {
            putchar( '.' );
            clock_ticks -= BLIP_COUNT;
        }
    }
    _dos_setvect( 0x1c, prev_int_1c );
}
