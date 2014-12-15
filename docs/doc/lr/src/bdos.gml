.func bdos
.synop begin
#include <&doshdr>
int bdos( int dos_func, unsigned dx, unsigned char al );
.ixfunc2 '&OsInt' &funcb
.synop end
.desc begin
The
.id &funcb.
function causes the computer's central processor (CPU) to
be interrupted with an interrupt number hexadecimal 21 (
.ct .mono 0x21
.ct ), which is
a request to invoke a specific DOS function.
Before the interrupt, the DX register is loaded from
.arg dx
.ct , the AH register is loaded with the DOS function number from
.arg dos_func
and the AL register is loaded from
.arg al
.ct .li .
The remaining registers are passed unchanged to DOS.
.np
You should consult the technical documentation for the DOS operating
system you are using to determine the expected register contents before
and after the interrupt in question.
.desc end
.return begin
The
.id &funcb.
function returns the value of the AX register after the
interrupt has completed.
.return end
.see begin
.im seeint
.see end
.exmp begin
#include <dos.h>

#define DISPLAY_OUTPUT  2
.exmp break
void main()
  {
    int rc;

    rc = bdos( DISPLAY_OUTPUT, 'B', 0 );
    rc = bdos( DISPLAY_OUTPUT, 'D', 0 );
    rc = bdos( DISPLAY_OUTPUT, 'O', 0 );
    rc = bdos( DISPLAY_OUTPUT, 'S', 0 );
  }
.exmp end
.class DOS
.system
