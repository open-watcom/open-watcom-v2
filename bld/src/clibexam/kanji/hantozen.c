#include <stdio.h>
#include <jstring.h>

char alphabet[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

void main()
  {
    int   i;
    JMOJI c;

    for( i = 0; i < sizeof( alphabet ) - 1; i++ ) {
        c = hantozen( alphabet[ i ] );
        printf( "%c%c", c>>8, c );
    }
    printf( "\n" );
  }
