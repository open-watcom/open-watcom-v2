#include <stdio.h>
#include <process.h>

void main()
  {
    char buffer[PATH_MAX];

    printf( "%s\n", _cmdname( buffer ) );
  }
