#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

int main()
{
    errno_t rc;
    size_t  ct;
    FILE    *fp1;
    FILE    *fp2;
    signed char    c = 'x';

    rc = fopen_s( &fp1, "file", "w" );
    ct = fwrite( &c, sizeof( c ), 1, fp1 );
    ct = fwrite( &c, sizeof( c ), 1, fp1 );
    ct = fwrite( &c, sizeof( c ), 1, fp1 );
    ct = fwrite( &c, sizeof( c ), 1, fp1 );
    ct = fwrite( &c, sizeof( c ), 1, fp1 );
    rc = freopen_s( &fp2, "file", "r", fp1 );
    if( rc == 0 ) {
      while( (c = fgetc( fp2 )) != EOF )
        fputchar(c);
      fclose( fp2 );
    }
    return( 0 );
}
