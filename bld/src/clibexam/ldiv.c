#include <stdio.h>
#include <stdlib.h>

void print_time( long int ticks )
  {
    ldiv_t sec_ticks;
    ldiv_t min_sec;

    sec_ticks = ldiv( ticks, 100L );
    min_sec   = ldiv( sec_ticks.quot, 60L );
    printf( "It took %ld minutes and %ld seconds\n",
             min_sec.quot, min_sec.rem );
  }

void main()
  {
    print_time( 86712L );
  }
