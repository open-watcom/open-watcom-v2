#include <stdio.h>
#include <dos.h>

void main()
  {
    unsigned drive;

    _dos_getdrive( &drive );
    printf( "The current drive is %c\n",
                'A' + drive - 1 );
  }
