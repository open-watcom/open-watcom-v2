#include <stdio.h>
#include <string.h>
#include <i86.h>

void main() /* Print location of Break Key Vector */
  {
    union REGPACK regs;

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
