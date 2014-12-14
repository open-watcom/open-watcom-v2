.func inpd
.synop begin
#include <conio.h>
unsigned long inpd( int port );
.ixfunc2 '&PortIo' &func
.synop end
.desc begin
The
.id &func.
function reads a double-word (four bytes) from the 80x86 hardware
port whose number is given by
.arg port
.ct .li .
.im portdesc
.im privity
.desc end
.return begin
The value returned is the double-word that was read.
.return end
.see begin
.seelist inp inpd inpw outp outpd outpw
.see end
.exmp begin
#include <conio.h>
#define DEVICE 34

void main()
  {
    unsigned long transmitted;
.exmp break
    transmitted = inpd( DEVICE );
  }
.exmp end
.class Intel
.system
