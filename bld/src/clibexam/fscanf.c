
#include <stdio.h>

void main()
  {
    int day, year;
    char weekday[10], month[10];
    FILE *in_data;

    in_data = fopen( "file", "r" );
    if( in_data != NULL ) {
      fscanf( in_data, "%s %s %d %d",
              weekday, month, &day, &year );
      printf( "Weekday=%s Month=%s Day=%d Year=%d\n",
              weekday, month, day, year );
      fclose( in_data );
    }
  }

