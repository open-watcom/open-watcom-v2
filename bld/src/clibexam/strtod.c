#include <stdio.h>
#include <stdlib.h>

void main()
  {
    double pi;

    pi = strtod( "3.141592653589793", NULL );
    printf( "pi=%17.15f\n",pi );
  }
