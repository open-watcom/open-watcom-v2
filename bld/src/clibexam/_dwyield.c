#include <wdefwin.h>
#include <stdio.h>

void main()
  {
    int i;

    for( i = 0; i < 1000; i++ ) {
      /* give other processes a chance to run */
      _dwYield();
      /* do CPU-intensive calculation */
      /*  .  */
      /*  .  */
      /*  .  */
    }
  }
