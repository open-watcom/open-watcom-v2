#include <stdio.h>
#include <jstring.h>

char * types[4] = {
    "CT_ILGL",
    "CT_ANK",
    "CT_KJ1",
    "CT_KJ2"
};

void main()
  {
    int     i;

    for( i = 0; i < 9; i++ )
        printf( "%s\n",
            types[ 1 + nthctype( "abA¡Bc", i ) ]
            );
  }
