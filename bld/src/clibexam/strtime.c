#include <stdio.h>
#include <time.h>

void main()
  {
    char timebuff[9];

    printf( "%s\n", _strtime( timebuff ) );
  }
