.func segread
.synop begin
#include <i86.h>
void segread( struct SREGS *seg_regs );
.synop end
.desc begin
The &func function places the values of the segment registers into
the structure located by
.arg seg_regs
.ct .li .
.desc end
.return begin
No value is returned.
.return end
.see begin
.seelist segread FP_OFF FP_SEG MK_FP
.see end
.exmp begin
#include <stdio.h>
#include <i86.h>

void main()
  {
    struct SREGS sregs;
.exmp break
    segread( &sregs );
    printf( "Current value of CS is %04X\n", sregs.cs );
  }
.exmp end
.class WATCOM
.system
