#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    'A',
    0x99A6,
    0xA4,
    0xD0
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;

    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa Katakana character\n",
            ( iskana( chars[i] ) ) ? "" : "not " );
    }
  }
