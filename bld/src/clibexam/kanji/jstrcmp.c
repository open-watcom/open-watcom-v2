#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrcmp( "abcdef", "abcdef" ) );
    printf( "%d\n", jstrcmp( "abcdef", "abc" ) );
    printf( "%d\n", jstrcmp( "abc", "abcdef" ) );
    printf( "%d\n", jstrcmp( "abcdef", "mnopqr" ) );
    printf( "%d\n", jstrcmp( "mnopqr", "abcdef" ) );
  }
