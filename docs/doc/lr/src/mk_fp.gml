.func MK_FP
#include <i86.h>
void __far *MK_FP( unsigned int segment,
                   unsigned int offset );
.funcend
.desc begin
The &func macro can be used to obtain the far
pointer value given by the
.arg segment
segment value and the
.arg offset
offset value.
These values may be obtained by using the
.kw FP_SEG
and
.kw FP_OFF
macros.
.desc end
.return begin
The macro returns a far pointer.
.return end
.see begin
.seelist MK_FP FP_OFF FP_SEG segread
.see end
.exmp begin
#include <i86.h>
#include <stdio.h>

void main()
  {
    unsigned short __far *bios_prtr_port_1;
.exmp break
    bios_prtr_port_1 =
            (unsigned short __far *) MK_FP( 0x40, 0x8 );
    printf( "Port address is %x\n", *bios_prtr_port_1 );
  }
.exmp end
.class Intel
.system
