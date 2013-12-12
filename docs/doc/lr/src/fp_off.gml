.func FP_OFF
.synop begin
#include <i86.h>
unsigned FP_OFF( void __far *far_ptr );
.synop end
.desc begin
The
.id &func.
macro can be used to obtain the offset portion of the far
pointer value given in
.arg far_ptr
.ct .li .
.desc end
.return begin
The macro returns an unsigned integer value which is the offset portion
of the pointer value.
.return end
.see begin
.seelist FP_OFF FP_SEG MK_FP segread
.see end
.exmp begin
#include <stdio.h>
#include <i86.h>

char ColourTable[256][3];

void main()
  {
    union REGPACK r;
    int i;
.exmp break
    /* read block of colour registers */
    r.h.ah = 0x10;
    r.h.al = 0x17;
#if defined(__386__)
    r.x.ebx = 0;
    r.x.ecx = 256;
    r.x.edx = FP_OFF( ColourTable );
    r.w.ds = r.w.fs = r.w.gs = FP_SEG( &r );
#else
    r.w.bx = 0;
    r.w.cx = 256;
    r.w.dx = FP_OFF( ColourTable );
#endif
    r.w.es = FP_SEG( ColourTable );
    intr( 0x10, &r );
.exmp break
    for( i = 0; i < 256; i++ ) {
      printf( "Colour index = %d "
              "{ Red=%d, Green=%d, Blue=%d }\n",
              i,
              ColourTable[i][0],
              ColourTable[i][1],
              ColourTable[i][2] );
    }
  }
.exmp end
.class Intel
.system
