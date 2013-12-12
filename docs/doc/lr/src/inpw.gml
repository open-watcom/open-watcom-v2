.func inpw
.synop begin
#include <conio.h>
unsigned int inpw( int port );
.ixfunc2 '&PortIo' &func
.synop end
.desc begin
The &func function reads a word (two bytes) from the 80x86 hardware
port whose number is given by
.arg port
.ct .li .
.im portdesc
.im privity
.desc end
.return begin
The value returned is the word that was read.
.return end
.see begin
.seelist &function. inp inpd inpw outp outpd outpw
.see end
.exmp begin
#include <conio.h>
#define DEVICE 34

void main()
  {
    unsigned int transmitted;
.exmp break
    transmitted = inpw( DEVICE );
  }
.exmp end
.class Intel
.system
