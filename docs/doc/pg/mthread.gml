:set symbol="oprompt"    value=&prompt.
.if '&targetos' eq 'OS/2 2.x' .do begin
:set symbol="prompt"    value="[C:\]".
:set symbol="tgtosname" value="os2v2".
.do end
.el .do begin
:set symbol="prompt"    value="C:\>".
:set symbol="tgtosname" value="nt".
.do end
:set symbol="tgtfile" value="mthread".
.*
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="tgtopts"   value="&sw.bm".
:set symbol="include" value="include".
:set symbol="begthread" value="beginthread".
:set symbol="endthread" value="endthread".
:set symbol="threadid" value="threadid".
:set symbol="threadacc" value="function".
:set symbol="null" value="0".
:set symbol="function" value="subroutine".
:set symbol="thrdincl" value="thread.fi".
:set symbol="idincl" value="thread.fi".
:set symbol="routine" value="subroutine".
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.   .if '&targetos' eq 'OS/2 2.x' .do begin
:set symbol="tgtopts"   value="&sw.bt=os2 &sw.bm".
.   .do end
.   .el .do begin
:set symbol="tgtopts"   value="&sw.bt=nt &sw.bm".
.   .do end
:set symbol="include" value="header".
:set symbol="begthread" value="_beginthread".
:set symbol="endthread" value="_endthread".
:set symbol="threadid" value="_threadid".
:set symbol="threadacc" value="variable/macro".
:set symbol="threadacc" value="macro".
:set symbol="null" value="NULL".
:set symbol="function" value="function".
:set symbol="thrdincl" value="process.h".
:set symbol="idincl" value="stddef.h".
:set symbol="routine" value="function".
.do end
.*
.chap &targetos Multi-threaded Applications
.*
.np
.ix 'multi-threaded applications' '&targetos.'
This chapter describes how to create multi-threaded applications.
A multi-threaded application is one whose tasks are divided among several
threads of execution.
.ix 'threads of execution'
A process is an executing application and the resources it uses.
.ix 'multi-threaded applications'
A thread is the smallest unit of execution within a process.
Each thread has its own stack and a set of machine registers and
shares all resources with its parent process.
The path of execution of one thread does not affect that of another;
each thread is an independent entity.
.np
Typically, an application has a single thread of execution.
In this type of application, all tasks, once initiated, are completed
before the next task begins.
In contrast, tasks in a multi-threaded application can be performed
concurrently since more than one thread is executing at once.
For example, each thread may be designed to perform a separate task.
.*
.section Programming Considerations
.*
.np
.ix 'multi-threading issues' '&targetos.'
Since a multi-threaded application consists of many threads of execution,
there are a number of issues that you must consider.
.np
Since threads share the resources of its parent, it may be necessary
to serialize access to these resources.
.if '&lang' eq 'FORTRAN 77' .do begin
For example, if your application contains more than one thread of
execution and each thread uses the
.kw PRINT
statement to display output to the console, it would be necessary for
the I/O support routines to allow only one thread to use the
.kw PRINT
facility at any time.
That is, once a thread issues a
.kw PRINT
request, the I/O support routines should ensure that no other thread
displays information until all information for the initial thread has
been displayed.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
For example, if your application has a function that displays
information on the console and is used by all threads, it is necessary
to allow only one thread to use that function at any time.
That is, once a thread calls that function, the function should ensure
that no other thread displays information until all information for
the initial thread has been displayed.
An example of such a function is the
.id printf
library function.
.np
Another issue that must be considered when creating multi-threaded
applications is global variables.
If you have global variables that contain thread-specific information,
there must be an instance of each global variable for each thread.
An example of such a variable is the
.id errno
global variable defined in the run-time libraries.
If an error condition was created by a thread, you would not want it
to affect the execution of other threads.
Therefore, each thread should contain its own instance of this
variable.
.do end
.*
.section Creating Threads
.*
.np
.ix 'thread creation' '&targetos.'
Each application initially contains a single thread.
The run-time libraries contain two functions that create and terminate
threads of execution.
The function
.id &begthread
creates a thread of execution and the function
.id &endthread
ends a thread of execution.
The &threadacc
.id &threadid
can be used to determine the current thread identifier.
.np
.warn
If any thread
.if '&lang' eq 'FORTRAN 77' .do begin
uses an I/O statement or calls an intrinsic function,
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
calls a library function,
.do end
you must use the
.id &begthread
function to create the thread.
Do not use the
.if '&targetos' eq 'OS/2 2.x' .do begin
.id DosCreateThread
.do end
.el .do begin
.id CreateThread
.do end
API function.
.ewarn
.*
.beglevel
.*
.section Creating a New Thread
.*
.np
.ix 'thread creation' '&targetos.'
.ix '&begthread function'
The
.id &begthread
function creates a new thread.
It is defined as follows.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
integer function beginthread( start_address,
                              stack_size )
integer stack_size
end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&targetos' eq 'OS/2 2.x' .do begin
.millust begin
int _beginthread( void (*start_address)(void *),
                  void *stack_bottom,
                  unsigned stack_size,
                  void *arglist );
.millust end
.do end
.el .do begin
.millust begin
unsigned long _beginthread( void (*start_address)(void *),
                            unsigned stack_size,
                            void *arglist);
.millust end
.do end
.do end
.synote
.mnote start_address
is the address of the &function that will be called when the newly
created thread is executed.
When the thread returns from that &function, the thread will be
terminated.
Note that a call to the
.id &endthread
&routine will also terminate the thread.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&targetos' eq 'OS/2 2.x' .do begin
.mnote stack_bottom
specifies the bottom of the stack to be used by the thread.
Note that this argument is ignored as it is only needed to simplify
the port of OS/2 1.x multi-threaded applications to &targetos..
Under &targetos, the operating system allocates the stack for the new
thread.
A value of NULL may be specified.
.do end
.do end
.mnote stack_size
specifies the size of the stack to be allocated by the operating
system for the new thread.
The stack size should be a multiple of 4K.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.mnote arglist
is passed as an argument to the &function specified by
.id start_address.
If no argument is required, a value of &null can be specified.
.do end
.esynote
.np
If a new thread is successfully created, the thread identifier of the
new thread is returned.
Otherwise, a value of -1 is returned.
.np
The &include file
.fi &thrdincl
contains the definition of the
.id &begthread
function.
.if '&targetos' eq 'Windows NT' .do begin
.np
Another thread related function for &targetos is
.id _beginthreadex.
See the
.book &company C Library Reference
for more information.
.do end
.*
.section Terminating the Current Thread
.*
.np
.ix 'thread termination' '&targetos.'
.ix '&endthread &routine'
The
.id &endthread
&routine terminates the current thread.
It is defined as follows.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
subroutine endthread()
end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
void _endthread( void )
.millust end
.do end
.np
The &include file
.fi &thrdincl
contains the definition of the
.id &endthread
function.
.*
.section Getting the Current Thread Identifier
.*
.np
.ix 'thread identifier' '&targetos.'
.ix '&threadid &threadacc'
The
.id &threadid
&threadacc can be used to determine the current thread identifier.
It is defined as follows.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
integer function threadid()
end
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
int *__threadid(void);
#define _threadid (__threadid())
.millust end
.do end
.np
The &include file
.fi &idincl
contains the definition of the
.id &threadid
&threadacc..
.*
.endlevel
.*
.section A Multi-threaded Example
.*
.np
.ix 'thread example' '&targetos.'
Let us create a simple multi-threaded application.
.if '&targetos' eq 'OS/2 2.x' .do begin
.if '&lang' eq 'FORTRAN 77' .do begin
The source code for this example can be found in
.fi &pathnam.\samples\fortran\os2.
.code begin
* MTHREAD.FOR

*$pragma aux DosSleep parm( value ) [] caller

      integer NumThreads
      logical HoldThreads
      common NumThreads, HoldThreads

      integer STACK_SIZE
      parameter (STACK_SIZE=32768)
      integer NUM_THREADS
      parameter (NUM_THREADS=5)

      integer i, threadid, beginthread
      external a_thread

.code break
      print '(''main thread id = '', i4)', threadid()
      NumThreads = 0
      HoldThreads = .true.
      ! main thread counts as 1
      do i = 2, NUM_THREADS
          if( beginthread( a_thread, STACK_SIZE ) .eq. -1 )then
              print '(''creation of thread'', i4, ''failed'')', i
          else
              NumThreads = NumThreads + 1
          end if
      end do
.code break
      HoldThreads = .false.
      while( NumThreads .ne. 0 )do
          call DosSleep( 1 )
      end while
      end

.code break
      subroutine a_thread()
      integer NumThreads
      logical HoldThreads
      common NumThreads, HoldThreads
      integer threadid
      while( HoldThreads )do
          call DosSleep( 1 )
      end while
.code break
      call DosEnterCritSec()
      print '(''Hi from thread '', i4)', threadid()
      NumThreads = NumThreads - 1
      call DosExitCritSec()
      call endthread()
      end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The source code for this example can be found in
.fi &pathnam.\samples\os2.
.code begin
#include <process.h>
#include <stdio.h>
#include <stddef.h>
#define INCL_DOS
#include <os2.h>

static  volatile int    NumThreads;
static  volatile int    HoldThreads;

#define NUM_THREADS     5
#define STACK_SIZE      32768

.code break
static void a_thread( void *arglist )
/***********************************/
{
    while( HoldThreads ) {
        DosSleep( 1 );
    }
    printf( "Hi from thread %d\n", *_threadid );
    DosEnterCritSec();
    --NumThreads;
    DosExitCritSec();
    _endthread();
}

.code break
int main( void )
/**************/
{
    int         i;

    printf( "Initial thread id = %d\n", *_threadid );
    NumThreads = 0;
    HoldThreads = 1;
    /* initial thread counts as 1 */
    for( i = 2; i <= NUM_THREADS; ++i ) {
        if( _beginthread( a_thread, NULL, STACK_SIZE, NULL ) == -1 ) {
            printf( "creation of thread %d failed\n", i );
        } else {
            ++NumThreads;
        }
    }
.code break
    HoldThreads = 0;
    while( NumThreads != 0 ) {
        DosSleep( 1 );
    }
    return( 0 );
}
.code end
.do end
.autonote Note:
.note
In the &function
.id a_thread,
.id DosEnterCritSec
and
.id DosExitCritSec
are called when we modify the variable
.id NumThreads.
This ensures that the action of extracting the value of
.id NumThreads
from memory, incrementing the value, and storing the new result into
memory, occurs without interruption.
If these functions were not called, it would be possible for two
threads to extract the value of
.id NumThreads
from memory before an update occurred.
.endnote
.do end
.*
.if '&targetos' eq 'Windows NT' .do begin
.if '&lang' eq 'FORTRAN 77' .do begin
The source code for this example can be found in
.fi &pathnam.\samples\fortran\win32.
.code begin
* MTHREAD.FOR

*$pragma aux (__stdcall) Sleep parm( value )
*$pragma aux (__stdcall) InitializeCriticalSection parm( reference )
*$pragma aux (__stdcall) DeleteCriticalSection parm( reference )
*$pragma aux (__stdcall) EnterCriticalSection parm( reference )
*$pragma aux (__stdcall) LeaveCriticalSection parm( reference )

        structure /RTL_CRITICAL_SECTION/
            integer*4 DebugInfo
            integer*4 LockCount
            integer*4 RecursionCount
            integer*4 OwningThread
            integer*4 LockSemaphore
            integer*4 Reserved
        end structure

.code break
        integer NumThreads
        logical HoldThreads
        volatile HoldThreads, NumThreads
        record /RTL_CRITICAL_SECTION/ CriticalSection
        common NumThreads, HoldThreads, CriticalSection

        integer STACK_SIZE
        parameter (STACK_SIZE=8192)
        integer NUM_THREADS
        parameter (NUM_THREADS=5)

        integer i, threadid, beginthread
        external a_thread

.code break
        print '(''main thread id = '',i4)', threadid()
        NumThreads = 0
        HoldThreads = .true.
        ! main thread counts as 1
        call InitializeCriticalSection( CriticalSection )
        do i = 2, NUM_THREADS
            if( beginthread( a_thread, STACK_SIZE ) .eq. -1 )then
                print '(''creation of thread'',i4,''failed'')', i
            else
                NumThreads = NumThreads + 1
            end if
        end do
.code break
        HoldThreads = .false.
        while( NumThreads .ne. 0 ) do
            call Sleep( 1 )
        end while
        call DeleteCriticalSection( CriticalSection )
        end

.code break
        subroutine a_thread()

        structure /RTL_CRITICAL_SECTION/
            integer*4 DebugInfo
            integer*4 LockCount
            integer*4 RecursionCount
            integer*4 OwningThread
            integer*4 LockSemaphore
            integer*4 Reserved
        end structure

.code break
        integer NumThreads
        logical HoldThreads
        volatile HoldThreads
        record /RTL_CRITICAL_SECTION/ CriticalSection
        common NumThreads, HoldThreads, CriticalSection

        integer threadid

.code break
        while( HoldThreads ) do
            call Sleep( 1 )
        end while
.code break
        print '(''Hi from thread '', i4)', threadid()
        call EnterCriticalSection( CriticalSection )
        NumThreads = NumThreads - 1
        call LeaveCriticalSection( CriticalSection )
        call endthread()
        end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
#include <process.h>
#include <stdio.h>
#include <stddef.h>
#include <windows.h>

static  volatile int    NumThreads;
static  volatile int    HoldThreads;

CRITICAL_SECTION CriticalSection;

#define NUM_THREADS     5
#define STACK_SIZE      8192

.code break
static void a_thread( void *arglist )
/***********************************/
{
    while( HoldThreads ) {
        Sleep( 1 );
    }
    printf( "Hi from thread %d\n", *_threadid );
    EnterCriticalSection( &CriticalSection );
    --NumThreads;
    LeaveCriticalSection( &CriticalSection );
    _endthread();
}

.code break
int main( void )
/**************/
{
    int         i;

    printf( "Initial thread id = %d\n", *_threadid );
    NumThreads = 0;
    HoldThreads = 1;
    InitializeCriticalSection( &CriticalSection );
    /* initial thread counts as 1 */
    for( i = 2; i <= NUM_THREADS; ++i ) {
        if( _beginthread( a_thread, STACK_SIZE, NULL ) == -1 ) {
            printf( "creation of thread %d failed\n", i );
        } else {
            ++NumThreads;
        }
    }
.code break
    HoldThreads = 0;
    while( NumThreads != 0 ) {
        Sleep( 1 );
    }
    DeleteCriticalSection( &CriticalSection );
    return( 0 );
}
.code end
.do end
.autonote Note:
.note
In the &function
.id a_thread,
.id EnterCriticalSection
and
.id LeaveCriticalSection
are called when we modify the variable
.id NumThreads.
This ensures that the action of extracting the value of
.id NumThreads
from memory, incrementing the value, and storing the new result into
memory, occurs without interruption.
If these functions were not called, it would be possible for two
threads to extract the value of
.id NumThreads
from memory before an update occurred.
.endnote
.do end
.np
Let us assume that the file
.fi &tgtfile..&langsuff
contains the above example.
Before compiling the file, make sure that the
.ev &pathvarup.
environment variable is set to the directory in which you installed
&product..
.if '&lang' eq 'FORTRAN 77' .do begin
Also, the
.ev FINCLUDE
environment variable must contain the
.if '&targetos' eq 'OS/2 2.x' .do begin
.fi &pathnam\src\fortran\os2
.do end
.el .do begin
.fi &pathnam\src\fortran
.do end
directory where "&pathnamup" is the name of the directory in which you
installed &product..
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Also, the
.ev INCLUDE
environment variable must include the
.if '&targetos' eq 'OS/2 2.x' .do begin
.fi &pathnam\h\os2
.do end
.el .do begin
.fi &pathnam\h\nt
.do end
and
.fi &pathnam\h
directories
("&pathnamup" is the directory in which &product was installed).
.do end
.np
We can now compile and link the application by issuing the following
command.
.millust begin
&prompt.&wclcmd &tgtopts &sw.l=&tgtosname &tgtfile
.millust end
.np
The "bm" option must be specified since we are creating a
multi-threaded application.
If your multi-threaded application contains more than one module, each
module must be compiled using the "bm" switch.
.np
The "l" option specifies the target system for which the application
is to be linked.
The system name
.id &tgtosname
is defined in the file
.fi wlsystem.lnk
which is located in the "BINW" subdirectory of the directory in which you
installed &product..
.np
The multi-threaded application is now ready to be run.
.*
.if '&targetos' eq 'OS/2 2.x' .do begin
.section Thread Limits
.*
.np
.ix 'thread limits' '&targetos.'
There is a limit to the number of threads an application can create
under 16-bit OS/2.
The default limit is 32.
.if '&lang' eq 'FORTRAN 77' .do begin
This limit can be adjusted by defining the integer function
.id __getmaxthreads
which returns the new thread limit.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
This limit can be adjusted by statically initializing the unsigned
global variable
.id __MaxThreads.
.do end
.np
Under 32-bit OS/2, there is no limit to the number of threads an
application can create.
However, due to the way in which multiple threads are supported in the
&company libraries, there is a small performance penalty once the number
of threads exceeds the default limit of 32 (this number includes the
initial thread).
If you are creating more than 32 threads and wish to avoid this
performance penalty, you can redefine the threshold value of 32.
You can statically initialize the global variable
.id __MaxThreads.
.np
.if '&lang' eq 'FORTRAN 77' .do begin
This limit can be adjusted by defining the integer function
.id __getmaxthreads
which returns the new thread limit.
By defining
.id __getmaxthreads
as follows, the new threshold value will be set to 48.
.millust begin
integer function __getmaxthreads()
__getmaxthreads = 48
end
.millust end
.np
This version of
.id __getmaxthreads
will replace the default function that is included in the run-time
library.
The default function simply returns the current value of the internal
variable
.id __MaxThreads.
Your version of this function will return a new value for this
variable.
Internally, the run-time system executes code similar to the
following:
.millust begin
    .
    .
    .
    __MaxThreads = __getmaxthreads()
    .
    .
    .
.millust end
.np
Thus, the default
.id __getmaxthreads
function does not alter the value of
.id __MaxThreads
but your version will.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
By adding the following line to your multi-threaded application, the
new threshold value will be set to 48.
.millust begin
unsigned __MaxThreads = { 48 };
.millust end
.do end
.*
.do end
.*
:set symbol="prompt"     value=&oprompt.
