#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>


void main()
  {
    unsigned char string[10];
    unsigned char *p;
    int           i;

    _setmbcp( 932 );
    p = string;
    _mbvtop( '.', p );
    p++;
    _mbvtop( '1', p );
    p++;
    _mbvtop( 'A', p );
    p++;
    _mbvtop( 0x8140, p );
    p += 2;
    _mbvtop( 0x8260, p );
    p += 2;
    _mbvtop( 0x82A6, p );
    p += 2;
    _mbvtop( '\0', p );

    for( i = 0; i < 10; i++ )
      printf( "%2.2x ", string[i] );
    printf( "\n" );
  }
