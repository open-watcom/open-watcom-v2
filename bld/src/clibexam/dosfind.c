#include <stdio.h>
#include <dos.h>

void main()
  {
    struct find_t  fileinfo;
    unsigned rc;        /* return code */

    /* Display name and size of "*.c" files */
    rc = _dos_findfirst( "*.c", _A_NORMAL, &fileinfo );
    while( rc == 0 ) {
      printf( "%14s %10ld\n", fileinfo.name,
                              fileinfo.size );
      rc = _dos_findnext( &fileinfo );
    }
    #if defined(__OS2__)
    _dos_findclose( &fileinfo );
    #endif
  }
