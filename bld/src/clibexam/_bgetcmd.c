
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

void main()
  {
    char *cmdline;
    int   cmdlen;

    cmdlen = _bgetcmd( NULL, 0 ) + 1;
    cmdline = malloc( cmdlen );
    if( cmdline != NULL ) {
      cmdlen = _bgetcmd( cmdline, cmdlen );
      printf( "%s\n", cmdline );
    }
  }

