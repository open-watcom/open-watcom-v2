#include <signal.h>

sig_atomic_t signal_count;

void MyHandler( int sig_number )
  {
     ++signal_count;
  }

void main()
  {
    signal( SIGFPE, MyHandler );   /* set own handler */
    signal( SIGABRT, SIG_DFL );    /* Default action */
    signal( SIGFPE, SIG_IGN );     /* Ignore condition */
  }
