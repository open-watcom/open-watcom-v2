#include <stdio.h>
#include <stdlib.h>

void print_time( int seconds )
  {
     auto div_t min_sec;

     min_sec = div( seconds, 60 );
     printf( "It took %d minutes and %d seconds\n",
              min_sec.quot, min_sec.rem );
  }

void main()
  {
    print_time( 130 );
  }
