#include <stdio.h>
#include <ctype.h>

char chars[] = {
    'A',
    0x09,
    ' ',
    0x7d
};

#define SIZE sizeof( chars ) / sizeof( char )

void main()
{
    int   i;

    for( i = 0; i < SIZE; i++ ) {
        printf( "Char %c is %sa space character\n",
                chars[i],
                ( isspace( chars[i] ) ) ? "" : "not " );
    }
}
