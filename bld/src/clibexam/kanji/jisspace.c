#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8140,
    0x8183,
    0x8254,
    0x8277
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisspace( c ) ) {
          printf( " is a space character" );
        }
        printf( "\n" );
    }
  }
