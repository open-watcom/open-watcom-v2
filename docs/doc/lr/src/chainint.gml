.func _chain_intr
.synop begin
#include <&doshdr>
void _chain_intr( void (__interrupt __far *func)() );
.ixfunc2 '&CpuInt' &func
.synop end
.desc begin
The
.id &func.
function is used at the end of an interrupt routine to
start executing another interrupt handler (usually the previous
handler for that interrupt).
When the interrupt handler designated by
.arg func
receives control, the stack and registers appear as though the
interrupt just occurred.
.desc end
.return begin
The
.id &func.
function does not return.
.return end
.see begin
.seelist _chain_intr _dos_getvect _dos_keep _dos_setvect
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>

volatile int clock_ticks;
void (__interrupt __far *prev_int_1c)();
#define BLIP_COUNT  (5*18)  /* 5 seconds */
.exmp break
void __interrupt __far timer_rtn()
  {
    ++clock_ticks;
    _chain_intr( prev_int_1c );
  }
.exmp break
int delays = 0;

int compile_a_line()
  {
    if( delays > 15 ) return( 0 );
    delay( 1000 );  /* delay for 1 second */
    printf( "Delayed for 1 second\n" );
    delays++;
    return( 1 );
  }
.exmp break
void main()
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
.exmp end
.class WATCOM
.system
