.func outp
.synop begin
#include <conio.h>
unsigned int outp( int port, int value );
.ixfunc2 '&PortIo' &func
.synop end
.desc begin
The
.id &func.
function writes one byte, determined by
.arg value
.ct , to the 80x86 hardware port whose number is given by
.arg port
.ct .li .
.im portdesc
.im privity
.desc end
.return begin
The value transmitted is returned.
.return end
.see begin
.seelist inp inpd inpw outp outpd outpw
.see end
.exmp begin
#include <conio.h>

void main()
  {
    /* turn off speaker */
    outp( 0x61, inp( 0x61 ) & 0xFC );
  }
.exmp end
.class Intel
.system
