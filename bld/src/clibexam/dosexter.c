#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

struct _DOSERROR dos_err;

void main()
  {
    int handle;

    /* Try to open "stdio.h" and then close it */
    if( _dos_open( "stdio.h", O_RDONLY, &handle ) != 0 ){
      dosexterr( &dos_err );
      printf( "Unable to open file\n" );
      printf( "exterror (AX) = %d\n", dos_err.exterror );
      printf( "errclass (BH) = %d\n", dos_err.errclass );
      printf( "action   (BL) = %d\n", dos_err.action );
      printf( "locus    (CH) = %d\n", dos_err.locus );
    } else {
      printf( "Open succeeded\n" );
      if( _dos_close( handle ) != 0 ) {
        printf( "Close failed\n" );
      } else {
        printf( "Close succeeded\n" );
      }
    }
  }
