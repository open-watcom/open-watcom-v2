#include <stdio.h>

static assemble_pass( int passno )
  {
    printf( "Pass %d\n", passno );
  }

void main()
  {
    FILE *fp;

    if( (fp = fopen( "program.asm", "r")) != NULL ) {
        assemble_pass( 1 );
        rewind( fp );
        assemble_pass( 2 );
        fclose( fp );
    }
  }
