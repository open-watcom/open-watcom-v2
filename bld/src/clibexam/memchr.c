#include <stdio.h>
#include <string.h>

void main()
  {
    char buffer[80];
    char *where;

    strcpy( buffer, "video x-rays" );
    where = (char *) memchr( buffer, 'x', 6 );
    if( where == NULL )
      printf( "'x' not found\n" );
    else
      printf( "%s\n", where );
    where = (char *) memchr( buffer, 'r', 9 );
    if( where == NULL )
      printf( "'r' not found\n" );
    else
      printf( "%s\n", where );
  }
