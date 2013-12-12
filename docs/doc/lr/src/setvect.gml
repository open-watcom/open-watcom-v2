.func _dos_setvect
#include <&doshdr>
void _dos_setvect( unsigned intnum,
                   void (__interrupt __far *handler)() );
.ixfunc2 '&CpuInt' &func
.synop end
.desc begin
The &func function sets interrupt vector number
.arg intnum
to point to the interrupt handling function pointed to by
.arg handler
.ct .li .
.desc end
.return begin
The &func function does not return a value.
.return end
.see begin
.seelist setvect _chain_intr _dos_getvect _dos_keep
.see end
.exmp begin
#include <stdio.h>
#include <dos.h>

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
int compile_a_line()
  {
    static int delays = 0;
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
