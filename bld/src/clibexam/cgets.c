#include <conio.h>

void main()
  {
    char buffer[82];

    buffer[0] = 80;
    cgets( buffer );
    cprintf( "%s\r\n", &buffer[2] );
  }
