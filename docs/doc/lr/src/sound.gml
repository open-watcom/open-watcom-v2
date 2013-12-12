.func sound
.synop begin
#include <i86.h>
void sound( unsigned frequency );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The
.id &func.
function turns on the PC's speaker at the specified
.arg frequency
.ct .li .
The frequency is in Hertz (cycles per second).
The speaker can be turned off by calling the
.kw nosound
function after an appropriate amount of time.
.im privity
.if '&machsys' eq 'QNX' .do begin
.bd WARNING:
The
.id &func.
function only works if either the program is owned by
.id root
and is
.id setuid,
or if the invoking user is
.id root.
.do end
.desc end
.return begin
The
.id &func.
function has no return value.
.return end
.see begin
.seelist sound delay nosound
.see end
.exmp begin
#include <i86.h>

/*
    The numbers in this table are the timer divisors
    necessary to produce the pitch indicated in the
    lowest octave that is supported by the "sound"
    function.

    To raise the pitch by N octaves, simply divide the
    number in the table by 2**N since a pitch which is
    an octave above another has double the frequency of
    the original pitch.

    The frequency obtained by these numbers is given by
    1193180 / X where X is the number obtained in the
    table.
*/
.exmp break
unsigned short Notes[] = {
        19327 ,        /* C b            */
        18242 ,        /* C              */
        17218 ,        /* C #   ( D b )  */
        16252 ,        /* D              */
        15340 ,        /* D #   ( E b )  */
        14479 ,        /* E     ( F b )  */
        13666 ,        /* F     ( E # )  */
        12899 ,        /* F #   ( G b )  */
        12175 ,        /* G              */
        11492 ,        /* G #   ( A b )  */
        10847 ,        /* A              */
        10238 ,        /* A #   ( B b )  */
        9664 ,         /* B     ( C b )  */
        9121 ,         /* B #            */
        0
};
.exmp break
#define FACTOR  1193180
#define OCTAVE  4

void main()             /* play the scale */
  {
    int i;
    for( i = 0; Notes[i]; ++i ) {
      sound( FACTOR / (Notes[i] / (1 << OCTAVE)) );
      delay( 200 );
      nosound();
    }
  }
.exmp end
.class Intel
.system
