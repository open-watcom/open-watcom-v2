#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8260,
    0x8261,
    0x8262,
    0x8263,
    0x8264
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = jtolower( chars[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
