.func signal
.synop begin
#include <signal.h>
void ( *signal(int sig, void (*func)(int)) )( int );
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The &func function is used to specify an action to take place when
certain conditions are detected while a program executes.
.if '&machsys' eq 'QNX' .do begin
See the
.hdrfile signal.h
header file for definitions of these conditions, and also refer to the
.us System Architecture
manual.
.do end
.el .do begin
These conditions are defined to be:
.begterm 12
.termhd1 Condition
.termhd2 Meaning
.term SIGABRT
abnormal termination, such as caused by the
.kw abort
function
.term SIGBREAK
an interactive attention (CTRL/BREAK on keyboard) is signalled
.term SIGFPE
an erroneous floating-point operation occurs (such as division by
zero, overflow and underflow)
.term SIGILL
illegal instruction encountered
.term SIGINT
an interactive attention (CTRL/C on keyboard) is signalled
.term SIGSEGV
an illegal memory reference is detected
.term SIGTERM
a termination request is sent to the program
.term SIGUSR1
OS/2 process flag A via DosFlagProcess
.term SIGUSR2
OS/2 process flag B via DosFlagProcess
.term SIGUSR3
OS/2 process flag C via DosFlagProcess
.endterm
.do end
.np
.if '&machsys' eq 'QNX' .do begin
There are three types of actions that can be associated with a signal:
.kw SIG_DFL
.ct ,
.kw SIG_IGN
.ct , or a
.us pointer to a function.
Initially, all signals are set to
.kw SIG_DFL
or
.kw SIG_IGN
prior to entry of the
.us main()
routine.
.do end
An action can be specified for each of the conditions, depending upon
the value of the
.arg func
argument:
.begterm 9
.term function
When
.arg func
is a function name, that function will be called equivalently to the
following code sequence.
.millust begin
    /* "sig_no" is condition being signalled */
    signal( sig_no, SIG_DFL );
    (*func)( sig_no );
.millust end
.pc
The
.arg func
function may terminate the program by calling the
.kw exit
or
.kw abort
functions or call the
.kw longjmp
function.
Because the next signal will be handled with default handling, the
program must again call &func if it is desired to handle the next
condition of the type that has been signalled.
.if '&machsys' eq 'QNX' .do begin
.np
If you use
.kw longjmp
to return from a signal handler, the signal will remain masked.
You can use
.kw siglongjmp
to restore the mask to the state saved in a previous call to
.kw sigsetjmp
.ct .li .
.do end
.np
After returning from the signal-catching function, the receiving
process will resume execution at the point at which it was
interrupted.
.np
The signal catching function is described as follows:
.millust begin
    void func( int sig_no )
    {

        /* body of function */

    }
.millust end
.if '&machsys' eq 'QNX' .do begin
.np
It is not possible to catch the signals
.kw SIGKILL
and
.kw SIGSTOP
.ct .li .
.do end
.np
Since signal-catching functions are invoked asynchronously with
process execution, the type
.kw sig_atomic_t
may be used to define variables on which an atomic operation (e.g.,
incrementation, decrementation) may be performed.
.term SIG_DFL
This value causes the default action for the condition to occur.
.if '&machsys' eq 'QNX' .do begin
.np
If the default action is to stop the process, the execution of that
process is temporarily suspended.
When a process stops, a
.kw SIGCHLD
signal is generated for its parent process, unless the parent process
has set the
.kw SA_NOCLDSTOP
flag (see
.kw sigaction
.ct ).
While a process is stopped, any additional signals that are sent to
the process are not delivered until the process is continued, except
.kw SIGKILL
.ct , which always terminates the receiving process.
:cmt. A process that is a member of an orphaned process group will not be
:cmt. allowed to stop in response to the
:cmt. .kw SIGTSTP
:cmt. .ct ,
:cmt. .kw SIGTTIN
:cmr. .ct , or
:cmt. .kw SIGTTOU
:cmt. signals.
:cmt. In cases where delivery of one of these signals would stop such a
:cmt. process, the signal is discarded.
.np
Setting a signal action to
.kw SIG_DFL
for a signal that is pending, and whose default action is to ignore
the signal (e.g.,
.kw SIGCHLD
.ct ), will cause the pending signal to be discarded, whether or not
it is blocked.
.do end
.term SIG_IGN
This value causes the indicated condition to be ignored.
.if '&machsys' eq 'QNX' .do begin
.np
The action for the signals
.kw SIGKILL
or
.kw SIGSTOP
cannot be set to
.kw SIG_IGN
.ct .li .
.np
Setting a signal action to
.kw SIG_IGN
for a signal that is pending will cause the pending signal to be
discarded, whether or not it is blocked.
.np
If a process sets the action for the
.kw SIGCHLD
signal to
.kw SIG_IGN
.ct , the behaviour is unspecified.
.do end
:cmt. .if '&machsys' ne 'QNX' .do begin
:cmt. .term SIG_ACK
:cmt. Used in OS/2 to acknowledge the receipt of a signal.
:cmt. Once a process receives a given signal, the operating system will
:cmt. not send any more signals of this type until it receives a
:cmt. .kw SIG_ACK
:cmt. acknowledgement back from the process.
:cmt. .do end
:cmt. .if '&machsys' eq 'QNX' .do begin
:cmt. .term SIG_HOLD
:cmt. This value causes the indicated condition to be held.
:cmt. .do end
.endterm
.np
When a condition is detected, it may be handled by a program,
it may be ignored, or it may be handled by the usual default action
(often causing an error message to be printed upon the
.filename stderr
stream followed by program termination).
.if '&machsys' ne 'QNX' .do begin
.np
When the program begins execution, the equivalent of
.millust begin
    signal( SIGABRT, SIG_DFL );
    signal( SIGFPE, SIG_DFL );
    signal( SIGILL, SIG_DFL );
    signal( SIGINT, SIG_DFL );
    signal( SIGSEGV, SIG_DFL );
    signal( SIGTERM, SIG_DFL );
    signal( SIGBREAK, SIG_DFL );
    signal( SIGUSR1, SIG_IGN );
    signal( SIGUSR2, SIG_IGN );
    signal( SIGUSR3, SIG_IGN );
.millust end
.pc
is executed.
.np
The
.kw SIGINT
signal is generated by pressing the CTRL/C or CTRL/BREAK key
combination on the keyboard.
Under DOS, if "BREAK=ON", a signal will be delivered at the next DOS
call; otherwise, if "BREAK=OFF", a signal will be delivered only at
the next standard input/output DOS call.
The
.kw BREAK
setting is configured in the
.filename CONFIG.SYS
file.
.np
Under OS/2, the
.kw SIGBREAK
signal can only be received if CTRL/BREAK is pressed and the keyboard is in 
binary (raw) mode. In ASCII (cooked) mode, which is the default, both CTRL/C
and CTRL/BREAK combinations will raise the 
.kw SIGINT
signal.
.do end
.np
A condition can be generated by a program using the
.kw raise
function.
.desc end
.return begin
A return value of
.kw SIG_ERR
indicates that the request could not be handled, and
.kw errno
is set to the value
.kw EINVAL
.ct .li .
.np
Otherwise, the previous value of
.arg func
for the indicated condition is returned.
.return end
.see begin
.seelist signal break... raise sigprocmask
.see end
.exmp begin
#include <stdio.h>
#include <signal.h>
#include <i86.h>

/* SIGINT Test */

sig_atomic_t signal_count;
sig_atomic_t signal_number;
.exmp break
void MyIntHandler( int signo )
{
    signal_count++;
    signal_number = signo;
}
.exmp break
void MyBreakHandler( int signo )
{
    signal_count++;
    signal_number = signo;
}
.exmp break
int main( void )
{
    int i;

    signal_count = 0;
    signal_number = 0;
    signal( SIGINT, MyIntHandler );
    signal( SIGBREAK, MyBreakHandler );
    printf( "Press Ctrl/C or Ctrl/Break\n" );
    for( i = 0; i < 50; i++ ) {
        printf( "Iteration # %d\n", i );
        delay( 500 ); /* sleep for 1/2 second */
        if( signal_count > 0 ) break;
    }
    printf( "SIGINT count %d number %d\n",
                    signal_count, signal_number );
.exmp break
    signal_count = 0;
    signal_number = 0;
    signal( SIGINT, SIG_DFL );      /* Default action */
    signal( SIGBREAK, SIG_DFL );    /* Default action */
    printf( "Default signal handling\n" );
    for( i = 0; i < 50; i++ ) {
        printf( "Iteration # %d\n", i );
        delay( 500 ); /* sleep for 1/2 second */
        if( signal_count > 0 ) break; /* Won't happen */
    }
    return( signal_count );
}
.exmp end
.class ANSI
.system
