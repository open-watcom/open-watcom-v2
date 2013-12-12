.func int86x
.synop begin
#include <i86.h>
int int86x( int inter_no,
            const union REGS *in_regs,
            union REGS *out_regs,
            struct SREGS *seg_regs );
.ixfunc2 '&CpuInt' &func
.synop end
.desc begin
The &func function causes the computer's central processor (CPU) to be
interrupted with an interrupt whose number is given by
.arg inter_no
.ct .li .
Before the interrupt, the CPU registers are loaded from the structure
located by
.arg in_regs
and the DS and ES segment registers are loaded from the structure
located by
.arg seg_regs
.ct .li .
.im segregs
.np
Following the interrupt, the structure located by
.arg out_regs
is filled with the contents of the CPU registers.
The
.arg in_regs
and
.arg out_regs
structures may be located at the same location in memory.
The original values of the DS and ES registers are restored.
The structure
.arg seg_regs
is updated with the values of the segment registers following the
interrupt.
.pp
You should consult the technical documentation for the computer that
you are using to determine the expected register contents before and
after the interrupt in question.
.desc end
.return begin
The function returns the value of the CPU AX register after the
interrupt.
.return end
.see begin
.im seeint int86x
.see end
.exmp begin
#include <stdio.h>
#include <i86.h>

/* get current mouse interrupt handler address */

void main()
  {
    union REGS r;
    struct SREGS s;
.exmp break
    r.h.ah = 0x35;  /* DOS get vector */
    r.h.al = 0x33;  /* interrupt vector 0x33 */
    int86x( 0x21, &r, &r, &s );
    printf( "mouse handler address=%4.4x:%4.4x\n",
            s.es, r.w.bx );
  }
.exmp end
.class Intel
.system
