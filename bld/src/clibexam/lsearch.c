#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

void main( int argc, const char *argv[] )
  {
    int i;
    unsigned num = 0;
    char **array = (char **)calloc( argc, sizeof(char **) );
    int compare( const void *, const void * );

    for( i = 1; i < argc; ++i ) {
      lsearch( &argv[i], array, &num, sizeof(char **),
                  compare );
    }
    for( i = 0; i < num; ++i ) {
      printf( "%s\n", array[i] );
    }
  }

int compare( const void *op1, const void *op2 )
  {
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
  }

/* With input: one two one three four */
