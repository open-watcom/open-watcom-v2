.func _dos_getvect
.synop begin
#include <&doshdr>
void (__interrupt __far *_dos_getvect(unsigned intnum))();
.ixfunc2 '&CpuInt' &funcb
.synop end
.desc begin
The
.id &funcb.
function gets the current value of interrupt vector number
.arg intnum
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns a far pointer to the current interrupt
handler for interrupt number
.arg intnum
.ct .li .
.return end
.see begin
.seelist getvect _chain_intr _dos_keep _dos_setvect
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

int delays = 0;
.exmp break
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
