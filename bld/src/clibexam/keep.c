#include <dos.h>

void permanent()
  {
    /* . */
    /* . */
    /* . */
  }

void transient()
  {
    /* . */
    /* . */
    /* . */
  }

void main()
  {
    /* initialize our TSR */
    transient();
    /*
        now terminate and keep resident
        the non-transient portion
    */
    _dos_keep( 0, (FP_OFF( transient ) + 15) >> 4 );
  }
