#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    'A',
    0x9941,
    0xA4,
    ' '
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;

    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa single-byte printable "
              "non-space character\n",
            ( isgrkana( chars[i] ) ) ? "" : "not " );
    }
  }
