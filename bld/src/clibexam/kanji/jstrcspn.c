#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%d\n", jstrcspn( "abcbcadef", "cba" ) );
    printf( "%d\n", jstrcspn( "xxxbcadef", "cba" ) );
    printf( "%d\n", jstrcspn( "123456789", "cba" ) );
  }
