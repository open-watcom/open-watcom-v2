#include <stdio.h>
#include <jstring.h>

JMOJI alphabet[26] = {
    '‚`', '‚a', '‚b', '‚c', '‚d', '‚e', '‚f', '‚g', '‚h',
    '‚i', '‚j', '‚k', '‚l', '‚m', '‚n', '‚o', '‚p', '‚q',
    '‚r', '‚s', '‚t', '‚u', '‚v', '‚w', '‚x', '‚y'
};

void main()
  {
    int   i;
    char  c;

    for( i = 0;
         i < sizeof( alphabet )/sizeof(JMOJI);
         i++ ) {
        c = zentohan( alphabet[ i ] );
        printf( "%c", c );
    }
    printf( "\n" );
  }
