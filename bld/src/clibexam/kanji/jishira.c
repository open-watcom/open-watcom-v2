#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x829F,
    0x82B0,
    0x82F1
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jishira( c ) ) {
          printf( " is a hiragana character" );
        }
        printf( "\n" );
    }
  }
