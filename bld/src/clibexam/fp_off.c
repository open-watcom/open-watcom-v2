#include <stdio.h>
#include <i86.h>

char ColourTable[256][3];

void main()
  {
    union REGPACK r;
    int i;

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

    for( i = 0; i < 256; i++ ) {
      printf( "Colour index = %d "
              "{ Red=%d, Green=%d, Blue=%d }\n",
              i,
              ColourTable[i][0],
              ColourTable[i][1],
              ColourTable[i][2] );
    }
  }
