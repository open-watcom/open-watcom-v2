#include <stdio.h>

char *weekday = { "Saturday" };
char *month = { "April" };

void main()
  {
    fprintf( stdout, "%s, %s %d, %d\n",
          weekday, month, 18, 1987 );
  }
