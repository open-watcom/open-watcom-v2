/*
 * Executes a given program, converting all
 * output to upper case.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char   buffer[256];

void main( int argc, char **argv )
  {
    int  i;
    int  c;
    FILE *f;

    for( i = 1; i < argc; i++ ) {
      strcat( buffer, argv[i] );
      strcat( buffer, " " );
    }

    if( ( f = _popen( buffer, "r" ) ) == NULL ) {
      perror( "_popen" );
      exit( 1 );
    }
    while( ( c = getc(f) ) != EOF ) {
      if( islower( c ) )
          c = toupper( c );
      putchar( c );
    }
    _pclose( f );
  }
