
#include <conio.h>

void main()
  {
    int day, year;
    char weekday[10], month[12];

    cscanf( "%s %s %d %d",
            weekday, month, &day, &year );
    cprintf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }

