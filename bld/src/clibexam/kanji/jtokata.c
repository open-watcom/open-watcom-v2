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
    JMOJI c1, c2;

    for( i = 0; i < SIZE; i++ ) {
        c1 = chars[ i ];
        c2 = jtokata( c1 );
        printf( "%c%c - %c%c\n", c1>>8, c1, c2>>8, c2 );
    }
  }
