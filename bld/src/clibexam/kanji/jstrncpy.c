#include <stdio.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[15];

    printf( "%s\n", jstrncpy( buffer, "abcdefg", 10 ) );
    printf( "%s\n", jstrncpy( buffer, "1234567",  6 ) );
    printf( "%s\n", jstrncpy( buffer, "abcdefg",  3 ) );
    printf( "%s\n", jstrncpy( buffer, "*******",  0 ) );
  }
