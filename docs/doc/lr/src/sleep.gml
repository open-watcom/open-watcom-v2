.func sleep
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
unsigned int sleep( unsigned int seconds );
.do end
.el .do begin
#include <&doshdr>
unsigned sleep( unsigned seconds );
.do end
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
.if '&machsys' eq 'QNX' .do begin
The &func function suspends the calling process until the number of
real time seconds specified by the
.arg seconds
argument have elapsed, or a signal whose action is to either terminate
the process or call a signal handler is received.
The suspension time may be greater than the requested amount due to
the scheduling of other, higher priority activity by the system.
.do end
.el .do begin
The &func function suspends execution by the specified number of
.arg seconds
.ct .li .
.do end
.desc end
.return begin
.if '&machsys' eq 'QNX' .do begin
The &func function returns zero if the full time specified was
completed; otherwise it returns the number of seconds unslept if
interrupted by a signal.
If an error occurs, an (unsigned)(-1) is returned and
.kw errno
will be set.
.do end
.el .do begin
The &func function always returns zero.
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
.seelist &function. alarm delay timer_create timer_gettime timer_settime
.see end
.exmp begin
/*
 * The following program sleeps for the
 * number of seconds specified in argv[1].
 */
#include <stdlib.h>
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
.do end
.el .do begin
#include <&doshdr>
.do end

void main( int argc, char *argv[] )
{
    unsigned seconds;
.exmp break
    seconds = (unsigned) strtol( argv[1], NULL, 0 );
    sleep( seconds );
}
.exmp end
.if '&machsys' eq 'QNX' .do begin
.class POSIX 1003.1
.do end
.el .do begin
.class WATCOM
.do end
.system
