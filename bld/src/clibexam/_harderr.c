#include <stdio.h>
#include <conio.h>
#include <dos.h>

int __far critical_error_handler( unsigned deverr,
                                  unsigned errcode,
                                  unsigned far *devhdr )
  {
    cprintf( "Critical error: " );
    cprintf( "deverr=%4.4X errcode=%d\r\n",
             deverr, errcode );
    cprintf( "devhdr = %Fp\r\n", devhdr );
    return( _HARDERR_IGNORE );
  }

main()
  {
    FILE *fp;

    _harderr( critical_error_handler );
    fp = fopen( "a:tmp.tmp", "r" );
    printf( "fp = %p\n", fp );
  }
