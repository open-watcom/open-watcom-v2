#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

int main()
{
    errno_t rc;
    FILE    *fp;

    rc = fopen_s( &fp, "file", "r" );
    if( fp != NULL ) {
      /* rest of code goes here */
      fclose( fp );
    }
    return( 0 );
}
