#include <stdio.h>
#include <stdlib.h>
#include <mbctype.h>

wchar_t wchar = { 0x0073 };
char    mbbuffer[2];

void main()
  {
    int len;

    _setmbcp( 932 );
    printf( "Character encodings are %sstate dependent\n",
            ( wctomb( NULL, 0 ) )
            ? "" : "not " );

    len = wctomb( mbbuffer, wchar );
    mbbuffer[len] = '\0';
    printf( "%s(%d)\n", mbbuffer, len );
  }
