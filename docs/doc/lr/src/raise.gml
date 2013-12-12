.func raise
#include <signal.h>
int raise( int condition );
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The &func function signals the exceptional condition indicated by the
.arg condition
argument.
The possible conditions are defined in the
.hdrfile signal.h
header file and are documented with the
.kw signal
function.
The
.kw signal
function can be used to specify the action which is to take
place when such a condition occurs.
.desc end
.return begin
The &func function returns zero when the condition is successfully
raised and a non-zero value otherwise.
There may be no return of control following the function call if the
action for that condition is to terminate the program or to transfer
control using the
.kw longjmp
function.
.return end
.see begin
.seelist raise signal
.see end
.exmp begin
/*
 * This program waits until a SIGINT signal
 * is received.
 */
#include <stdio.h>
#include <signal.h>

sig_atomic_t signal_count;
sig_atomic_t signal_number;
.exmp break
static void alarm_handler( int signum )
  {
    ++signal_count;
    signal_number = signum;
  }
.exmp break
void main()
  {
    unsigned long i;

    signal_count = 0;
    signal_number = 0;
    signal( SIGINT, alarm_handler );
.exmp break
    printf("Signal will be auto-raised on iteration "
           "10000 or hit CTRL-C.\n");
    printf("Iteration:      ");
    for( i = 0; i < 100000; ++i )
    {
      printf("\b\b\b\b\b%*d", 5, i);

      if( i == 10000 ) raise(SIGINT);

      if( signal_count > 0 ) break;
    }
.exmp break
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
.exmp end
.class ANSI
.system
