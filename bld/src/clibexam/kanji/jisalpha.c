#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8260,
    0x8279,
    0x8281,
    0x829A
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisalpha( c ) ) {
          printf( " is alphabetic" );
        }
        printf( "\n" );
    }
  }
