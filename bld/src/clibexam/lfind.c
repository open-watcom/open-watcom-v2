#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

static const char *keywords[] = {
        "auto",
        "break",
        "case",
        "char",
        /* . */
        /* . */
        /* . */
        "while"
};

void main( int argc, const char *argv[] )
  {
    unsigned num = 5;
    int compare( const void *, const void * );

    if( argc <= 1 ) exit( EXIT_FAILURE );
    if( lfind( &argv[1], keywords, &num, sizeof(char **),
                    compare ) == NULL ) {
      printf( "'%s' is not a C keyword\n", argv[1] );
      exit( EXIT_FAILURE );
    } else {
      printf( "'%s' is a C keyword\n", argv[1] );
      exit( EXIT_SUCCESS );
    }
  }

int compare( const void *op1, const void *op2 )
  {
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
  }
