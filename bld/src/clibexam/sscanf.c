#include <stdio.h>

/* Scan a date in the form "Saturday April 18 1987" */

void main()
  {
    int day, year;
    char weekday[20], month[20];

    sscanf( "Friday August 0014 1987",
            "%s %s %d  %d",
             weekday, month, &day, &year );
    printf( "%s %s %d %d\n",
             weekday, month, day, year );
  }
