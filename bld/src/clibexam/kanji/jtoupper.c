#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8281,
    0x8282,
    0x8283,
    0x8284,
    0x8285
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = jtoupper( chars[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
