#include <stdio.h>
#include <ctype.h>
#include <conio.h>

void main()
  {
    int c;
    long value;

    value = 0;
    c = getche();
    while( isdigit( c ) ) {
        value = value*10 + c - '0';
        c = getche();
    }
    ungetch( c );
    printf( "Value=%ld\n", value );
  }
