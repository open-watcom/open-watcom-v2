#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x002C,
    0x8183,
    0x0035,
    0x8277
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        if( jiszen( c ) ) {
          printf( "%c%c", c>>8, c );
          printf( " is a double-byte character" );
        } else {
          printf( "%c", c );
          printf( " is a single-byte character" );
        }
        printf( "\n" );
    }
  }
