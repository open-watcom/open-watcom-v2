#include <stdio.h>
#include <jstring.h>

void main()
  {
    JSTRING str;

    str = jstrskip( "Gone for lunch", "aeiou" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
    str = jstrskip( "Cat crazy", "ABCDEFabcdef" );
    if( str != NULL )
        printf( "%s\n", str );
    else
        printf( "Not found\n" );
  }
