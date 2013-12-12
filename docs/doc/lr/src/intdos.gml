.func intdos
#include <&doshdr>
int intdos( const union REGS *in_regs,
            union REGS *out_regs );
.ixfunc2 '&OsInt' &func
.synop end
.desc begin
The &func function causes the computer's central processor (CPU) to
be interrupted with an interrupt number hexadecimal 21 (
.ct .mono 0x21
.ct ), which is
a request to invoke a specific DOS function.
Before the interrupt, the CPU registers are loaded from the structure
located by
.arg in_regs
.ct .li .
The AH register contains a number indicating the function requested.
Following the interrupt, the structure located by
.arg out_regs
is filled with the contents of the CPU registers.
These structures may be located at the same location in memory.
.pp
You should consult the technical documentation for the DOS operating
system that you are using
to determine the expected register contents before
and after the interrupt in question.
.desc end
.return begin
The function returns the value of the AX (EAX in 386 library)
register after the interrupt has completed.
The CARRY flag (when set, an error has occurred) is copied into the
structure located by
.arg out_regs
.ct .li .
.im errnoref
.return end
.see begin
.im seeint intdos
.see end
.exmp begin
#include <dos.h>

#define DISPLAY_OUTPUT  2
.exmp break
void main()
  {
    union REGS  in_regs, out_regs;
    int         rc;
.exmp break
    in_regs.h.ah = DISPLAY_OUTPUT;
    in_regs.h.al = 0;
.exmp break
    in_regs.w.dx = 'I';
    rc = intdos( &in_regs, &out_regs );
    in_regs.w.dx = 'N';
    rc = intdos( &in_regs, &out_regs );
    in_regs.w.dx = 'T';
    rc = intdos( &in_regs, &out_regs );
    in_regs.w.dx = 'D';
    rc = intdos( &in_regs, &out_regs );
    in_regs.w.dx = 'O';
    rc = intdos( &in_regs, &out_regs );
    in_regs.w.dx = 'S';
    rc = intdos( &in_regs, &out_regs );
  }
.exmp end
.class DOS
.system
