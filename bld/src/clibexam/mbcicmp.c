#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned char mb1[2] = {
    0x41, 0x42
};

unsigned char mb2[2] = {
    0x61, 0x43
};

void main()
  {
    int     i;

    _setmbcp( 932 );
    i = _mbcicmp( mb1, mb2 );
    if( i == _NLSCMPERROR )
        printf( "Error in multibyte character\n" );
    else if( i < 0 )
        printf( "Less than\n" );
    else if( i == 0 )
        printf( "Equal to\n" );
    else
        printf( "Greater than\n" );

  }
