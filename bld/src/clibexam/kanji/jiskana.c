#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8340,
    0x8364,
    0x8396
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jiskana( c ) ) {
          printf( " is a katakana character" );
        }
        printf( "\n" );
    }
  }
