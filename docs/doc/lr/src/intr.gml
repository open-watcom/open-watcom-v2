.func intr
.synop begin
#include <i86.h>
void intr( int inter_no, union REGPACK *regs );
.ixfunc2 '&CpuInt' &func
.synop end
.desc begin
The
.id &func.
function causes the computer's central processor (CPU) to
be interrupted with an interrupt whose number is given by
.arg inter_no
.ct .li .
Before the interrupt, the CPU registers are loaded from the structure
located by
.arg regs
.ct .li .
All of the segment registers must contain valid values.
Failure to do so will cause a segment violation when running
in protect mode.
If you don't care about a particular segment register, then it
can be set to 0 which will not cause a segment violation.
Following the interrupt, the structure located by
.arg regs
is filled with the contents of the CPU registers.
.np
This function is similar to the
.kw int86x
function, except that only one structure is used for the register
values and that the BP (EBP in 386 library) register is included in
the set of registers that are passed and saved.
.np
You should consult the technical documentation for the computer that
you are using to determine the expected register contents before and
after the interrupt in question.
.desc end
.return begin
The
.id &func.
function does not return a value.
.return end
.see begin
.im seeint intr
.see end
.if '&machsys' ne 'QNX' .do begin
.exmp begin
#include <stdio.h>
#include <string.h>
#include <i86.h>

void main() /* Print location of Break Key Vector */
  {
    union REGPACK regs;
.exmp break
    memset( &regs, 0, sizeof(union REGPACK) );
    regs.w.ax = 0x3523;
    intr( 0x21, &regs );
    printf( "Break Key vector is "
#if defined(__386__)
            "%x:%lx\n", regs.w.es, regs.x.ebx );
#else
            "%x:%x\n", regs.w.es, regs.x.bx );
#endif
  }
.exmp output
Break Key vector is eef:13c
.exmp end
.do end
.class Intel
.system
