.func delay
#include <i86.h>
.if '&machsys' eq 'QNX' .do begin
unsigned int delay( unsigned int milliseconds );
.do end
.el .do begin
void delay( unsigned milliseconds );
.do end
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
.if '&machsys' eq 'QNX' .do begin
The &func function suspends the calling process until the number
of real time milliseconds specified by the
.arg milliseconds
argument have elapsed, or a signal whose action is to either terminate
the process or call a signal handler is received.
The suspension time may be greater than the requested amount due to
the scheduling of other, higher priority activity by the system.
.do end
.el .do begin
The &func function suspends execution by the specified number of
.arg milliseconds
.ct .li .
.do end
.desc end
.return begin
.if '&machsys' eq 'QNX' .do begin
The &func function returns zero if the full time specified was
completed; otherwise it returns the number of milliseconds unslept if
interrupted by a signal.
.do end
.el .do begin
The &func function has no return value.
.do end
.return end
.if '&machsys' eq 'QNX' .do begin
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EAGAIN
No timer resources available to satisfy the request.
.endterm
.error end
.do end
.see begin
.seelist delay alarm sleep
.see end
.exmp begin
#include <i86.h>

void main()
  {
    sound( 200 );
    delay( 500 );  /* delay for 1/2 second */
    nosound();
  }
.exmp end
.class WATCOM
.system
