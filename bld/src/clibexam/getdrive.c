#include <stdio.h>
#include <direct.h>

void main()
  {
    unsigned drive;

    printf( "The current drive is %c\n",
                'A' + _getdrive() - 1 );
  }
