.func int386x
.synop begin
#include <i86.h>
int int386x( int inter_no,
             const union REGS *in_regs,
             union REGS *out_regs,
             struct SREGS *seg_regs );
.ixfunc2 '&CpuInt' &func
.synop end
.desc begin
The
.id &func.
function causes the computer's central processor (CPU) to
be interrupted with an interrupt whose number is given by
.arg inter_no
.ct .li .
This function is present in the 32-bit C libraries and may be executed
on Intel 386 compatible systems.
Before the interrupt, the CPU registers are loaded from the structure
located by
.arg in_regs
and the DS, ES, FS and GS segment registers are loaded from the
structure located by
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
The original values of the DS, ES, FS and GS registers are restored.
The structure
.arg seg_regs
is updated with the values of the segment registers following the
interrupt.
.np
You should consult the technical documentation for the computer that
you are using to determine the expected register contents before and
after the interrupt in question.
.desc end
.return begin
The
.id &func.
function returns the value of the CPU EAX register after the
interrupt.
.return end
.see begin
.im seeint int386x
.see end
.exmp begin
#include <stdio.h>
#include <i86.h>
.exmp break
/* get current mouse interrupt handler address */

void main()
  {
    union REGS r;
    struct SREGS s;
.exmp break
    s.ds = s.es = s.fs = s.gs = FP_SEG( &s );
.exmp break
#if defined(__PHARLAP__)
    r.w.ax = 0x2503;    /* get real-mode vector */
    r.h.cl = 0x33;      /* interrupt vector 0x33 */
    int386( 0x21, &r, &r );
    printf( "mouse handler real-mode address="
            "%lx\n", r.x.ebx );
    r.w.ax = 0x2502;    /* get protected-mode vector */
    r.h.cl = 0x33;      /* interrupt vector 0x33 */
    int386x( 0x21, &r, &r, &s );
    printf( "mouse handler protected-mode address="
            "%x:%lx\n", s.es, r.x.ebx );
.exmp break
#else
    r.h.ah = 0x35;  /* get vector */
    r.h.al = 0x33;  /* vector 0x33 */
    int386x( 0x21, &r, &r, &s );
    printf( "mouse handler protected-mode address="
            "%x:%lx\n", s.es, r.x.ebx );
#endif
  }
.exmp end
.class Intel
.system
