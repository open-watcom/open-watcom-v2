#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <stdlib.h>

int  main()
{
    char    *wc = "string";
    wchar_t wbuffer[50];
    int     i;
    errno_t rc;
    size_t  retval;

    rc = mbstowcs_s( &retval, wbuffer, 50, wc, 10);
    if( rc == 0 ) {
      wbuffer[retval] = L'\0';
      printf( "%s(%d)\n", wc, retval );
      for( i = 0; i < retval; i++ )
        printf( "/%4.4x", wbuffer[i] );
      printf( "\n" );
    }
    return( 0 );
}
