#include <stdio.h>
#include <jstring.h>

void main()
  {
    JSTRING str;

    str = jstrmatch( "Gone for lunch", "aeiou" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
    str = jstrmatch( "Gone for lunch", "xyz" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
  }
