#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    'A',        /* single-byte A */
    'B',        /* single-byte B */
    'C',        /* single-byte C */
    'D',        /* single-byte D */
    'E',        /* single-byte E */
    0x8260,     /* double-byte A */
    0x8261,     /* double-byte B */
    0x8262,     /* double-byte C */
    0x8263,     /* double-byte D */
    0x8264      /* double-byte E */
};

#define SIZE sizeof( chars ) / sizeof( unsigned int )

void main()
  {
    int   i;
    unsigned int c;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      c = _mbctolower( chars[ i ] );
      if( c > 0xff )
        printf( "%c%c", c>>8, c );
      else
        printf( "%c", c );
    }
    printf( "\n" );
  }
