#include <stdio.h>
#include <io.h>

void main()
  {
    printf( "stdin is a %stty\n",
            ( isatty( fileno( stdin ) ) )
            ? "" : "not " );
  }
