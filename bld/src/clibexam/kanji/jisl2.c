#include <stdio.h>
#include <jstring.h>

JMOJI chars[] = {
    0x8183,
    0x889F,
    0x989F,
    0xEA9E
};

#define SIZE sizeof( chars ) / sizeof( JMOJI )

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < SIZE; i++ ) {
        c = chars[ i ];
        printf( "%c%c", c>>8, c );
        if( jisl2( c ) ) {
          printf( " is Level 2 JIS" );
        }
        printf( "\n" );
    }
  }
