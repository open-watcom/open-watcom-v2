#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstricmp( "AbCDEF", "abcdef" ) );
    printf( "%d\n", jstricmp( "abcdef", "ABC"    ) );
    printf( "%d\n", jstricmp( "abc",    "ABCdef" ) );
    printf( "%d\n", jstricmp( "Abcdef", "mnopqr" ) );
    printf( "%d\n", jstricmp( "Mnopqr", "abcdef" ) );
  }
