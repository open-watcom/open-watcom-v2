#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    'A',
    0x9941,
    0xA4,
    0xA6
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;

    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa single-byte "
              "Katakana punctuation character\n",
            ( iskpun( chars[i] ) ) ? "" : "not " );
    }
  }
