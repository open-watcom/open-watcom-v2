.func intdosx
.synop begin
#include <&doshdr>
int intdosx( const union REGS *in_regs,
             union REGS *out_regs,
             struct SREGS *seg_regs );
.ixfunc2 '&OsInt' &func
.synop end
.desc begin
The
.id &func.
function causes the computer's central processor (CPU) to
be interrupted with an interrupt number hexadecimal 21 (
.ct .mono 0x21
.ct ), which is
a request to invoke a specific DOS function.
Before the interrupt, the CPU registers are loaded from the structure
located by
.arg in_regs
and the segment registers DS and ES are loaded from the structure
located by
.arg seg_regs
.ct .li .
The AH register contains a number indicating the function requested.
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
The original values for the DS and ES registers are restored.
The structure
.arg seg_regs
is updated with the values of the segment registers following the
interrupt.
.np
You should consult the technical documentation for the DOS operating
system that you are using
to determine the expected register contents before
and after the interrupt in question.
.desc end
.return begin
The
.id &func.
function returns the value of the AX (EAX in 32-bit library)
register after the interrupt has completed.
The CARRY flag (when set, an error has occurred) is copied into the
structure located by
.arg out_regs
.ct .li .
.im errnoref
.return end
.see begin
.im seeint intdosx
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>

/* get current mouse interrupt handler address */
.exmp break
void main()
  {
    union REGS r;
    struct SREGS s;
.exmp break
#if defined(__386__)
    s.ds = s.es = s.fs = s.gs = FP_SEG( &s );
#endif
    r.h.ah = 0x35;  /* get vector */
    r.h.al = 0x33;  /* vector 0x33 */
    intdosx( &r, &r, &s );
#if defined(__386__)
    printf( "mouse handler address=%4.4x:%lx\n",
            s.es, r.x.ebx );
#else
    printf( "mouse handler address=%4.4x:%4.4x\n",
            s.es, r.x.bx );
#endif
  }
.exmp end
.class DOS
.system
