
#include <stdio.h>
#include <process.h>

void main()
  {
    char cmds[128];

    printf( "%s\n", getcmd( cmds ) );
  }

