#include <stdio.h>
#include <dos.h>

/* get current mouse interrupt handler address */

void main()
  {
    union REGS r;
    struct SREGS s;

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
