#include <stdio.h>
#include <stdlib.h>

void main()
{
  char *buf;
  FILE *fp;

  fp = fopen( "file", "r" );
  buf = malloc( 1024 );
  setvbuf( fp, buf, _IOFBF, 1024 );
}
