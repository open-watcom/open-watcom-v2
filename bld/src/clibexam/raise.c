/*
 * This program waits until a SIGINT signal
 * is received.
 */
#include <stdio.h>
#include <signal.h>

sig_atomic_t signal_count;
sig_atomic_t signal_number;

static void alarm_handler( int signum )
  {
    ++signal_count;
    signal_number = signum;
  }

void main()
  {
    unsigned long i;

    signal_count = 0;
    signal_number = 0;
    signal( SIGINT, alarm_handler );

    printf("Signal will be auto-raised on iteration "
           "10000 or hit CTRL-C.\n");
    printf("Iteration:      ");
    for( i = 0; i < 100000; ++i )
    {
      printf("\b\b\b\b\b%*d", 5, i);

      if( i == 10000 ) raise(SIGINT);

      if( signal_count > 0 ) break;
    }

    if( i == 100000 ) {
      printf("\nNo signal was raised.\n");
    } else if( i == 10000 ) {
      printf("\nSignal %d was raised by the "
              "raise() function.\n", signal_number);
    } else {
      printf("\nUser raised the signal.\n",
              signal_number);
    }
  }
