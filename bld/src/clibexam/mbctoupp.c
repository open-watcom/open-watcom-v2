#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned int chars[] = {
    'a',        /* single-byte a */
    'b',        /* single-byte b */
    'c',        /* single-byte c */
    'd',        /* single-byte d */
    'e',        /* single-byte e */
    0x8281,     /* double-byte a */
    0x8282,     /* double-byte b */
    0x8283,     /* double-byte c */
    0x8284,     /* double-byte d */
    0x8285      /* double-byte e */
};

#define SIZE sizeof( chars ) / sizeof( unsigned int )

void main()
  {
    int   i;
    unsigned int c;

    _setmbcp( 932 );
    for( i = 0; i < SIZE; i++ ) {
      c = _mbctoupper( chars[ i ] );
      if( c > 0xff )
        printf( "%c%c", c>>8, c );
      else
        printf( "%c", c );
    }
    printf( "\n" );
  }
