#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8143,
    0x817B,
    0x8183,
    0x8190
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jiskigou( c ) ) {
          printf( " is a kigou symbol" );
        }
        printf( "\n" );
    }
  }
