.func begin _beginthread
.func2 _beginthreadex
.func gen
#include <process.h>
.if &version ge 107 .do begin
#if defined(__386__)
#   define FAR
#else
#   define FAR __far
#endif

#if defined(__NT__)
unsigned long _beginthread(
        void (*start_address)(void *),
        unsigned stack_size,
        void     *arglist);
.ixfunc2 '&NTFunc' &func
.ixfunc2 '&TNTFunc' &func
unsigned long _beginthreadex(
        void *security,
        unsigned stack_size,
        unsigned (__stdcall *start_address)(void *),
        void *arglist,
        unsigned initflag,
        unsigned *thrdid );
.ixfunc2 '&NTFunc' &func.ex
.ixfunc2 '&TNTFunc' &func.ex
#else
int FAR _beginthread(
        void (FAR *start_address)(void FAR *),
        void FAR *stack_bottom,
        unsigned stack_size,
        void FAR *arglist );
.ixfunc2 '&OS2Func' &func
#endif
.do end
.el .do begin
int _beginthread(
        void (*start_address)(void *),
        void *stack_bottom,
        unsigned stack_size,
        void *arglist );
.ixfunc2 '&OS2Func' &func
.ixfunc2 '&NTFunc' &func
.ixfunc2 '&TNTFunc' &func
unsigned long _beginthreadex(
        void *security,
        unsigned stack_size,
        unsigned (__stdcall *start_address)(void *),
        void *arglist,
        unsigned initflag,
        unsigned *thrdid );
.ixfunc2 '&NTFunc' &func.ex
.ixfunc2 '&TNTFunc' &func.ex
.do end
.func end
.desc begin
The &func function is used to start a new thread of execution at the
function identified by
.arg start_address
with a single parameter identified by
.arg arglist
.ct .li .
.np
For each operating environment under which &func is supported,
the &func function uses the appropriate system call to begin a new
thread of execution.
.np
The new thread will use the memory identified by
.arg stack_bottom
and
.arg stack_size
for its stack.
.np
.us Note for 16-bit applications:
If the stack is not in DGROUP (i.e., the stack pointer does not point
to an area in DGROUP) then you must compile your application with the
"zu" option.
For example, the pointer returned by
.kw malloc
in a large data model may not be in DGROUP.
The "zu" option relaxes the restriction that the SS register
contains the base address of the default data segment, "DGROUP".
Normally, all data items are placed into the group DGROUP and the SS
register contains the base address of this group.
In a thread, the SS register will likely not contain the base address
of this group.
When the "zu" option is selected, the SS register is volatile (assumed
to point to another segment) and any global data references require
loading a segment register such as DS with the base address of
DGROUP.
.np
.us Note for OS/2 32-bit applications:
Memory for a stack need not be provided by the application.
The
.arg stack_bottom
may be NULL in which case the run-time system will provide a stack.
You must specify a non-zero
.arg stack_size
for this stack.
.np
.us Note for Win32 applications:
Memory for a stack is provided by the run-time system.
The size of the stack is determined by
.arg stack_size
and must not be zero.
.np
The
.kw _beginthreadex
function can be used to create a new thread,
in a running or suspended state specified by
.arg initflag
.ct .li ,
with security attributes specified by
.arg security
.ct .li .
.np
The initial state of the new thread (running or suspended) is
specified by the
.arg initflag
argument.
If the
.kw CREATE_SUSPENDED
flag (WINBASE.H) is specified, the thread is created in a suspended
state, and will not run until the Win32
.kw ResumeThread
function is called with the thread handle as an argument.
If this value is zero, the thread runs immediately after creation.
.np
The security descriptor for the new thread is specified by the
.arg security
argument.
This is a pointer to a Win32
.kw SECURITY_ATTRIBUTES
structure (see Microsoft's
.us Win32 Programmer's Reference
for more information).
For default behaviour, the security structure pointer can be
.kw NULL
.ct .li .
.np
The thread identifier is returned in the location identified by the
.arg thrdid
argument.
.np
The thread ends when it exits from its main function or calls
.kw exit
.ct,
.kw _exit
.ct,
.kw _endthread
or
.kw _endthreadex
.ct .li .
.np
The variable/function
.kw _threadid
which is defined in
.hdrfile stddef.h
may be used by the executing thread to obtain its thread ID.
In the 16-bit libraries,
.kw __threadid
is a far pointer to an int.
In the 32-bit libraries, it is a function that returns an int.
.np
There is no limit to the number of threads an application can create
under Win32 platforms.
.np
There is a limit to the number of threads an application can create
under 16-bit OS/2 and 32-bit NetWare.
The default limit is 32.
This limit can be adjusted by statically initializing the unsigned
global variable
.id __MaxThreads.
.np
Under 32-bit OS/2, there is no limit to the number of threads an
application can create.
However, due to the way in which multiple threads are supported in the
&company libraries, there is a small performance penalty once the
number of threads exceeds the default limit of 32 (this number
includes the initial thread).
If you are creating more than 32 threads and wish to avoid this
performance penalty, you can redefine the threshold value of 32.
You can statically initialize the global variable
.id __MaxThreads.
.np
By adding the following line to your multi-threaded application, the
new threshold value will be set to 48.
.millust begin
unsigned __MaxThreads = { 48 };
.millust end
.desc end
.return begin
Under Win32, the &func function returns the thread handle for the new
thread if successful; otherwise it returns &minus.1 to indicate that the
thread could not be started.
.np
Under all other systems that support the &func function (OS/2, Netware
and QNX), it returns the thread ID for the new thread if successful;
otherwise it returns &minus.1 to indicate that the thread could not be
started.
.np
The
.kw _beginthreadex
function returns the thread handle for the new thread if successful;
otherwise it returns 0 to indicate that the thread could not be
started.
.np
When the thread could not be started, the value of
.kw errno
could be set to
.kw EAGAIN
if there are too many threads, or to
.kw EINVAL
if the argument is invalid or the stack size is incorrect, or to
.kw ENOMEM
if there is not enough available memory.
.return end
.see begin
.seelist &function. _beginthread _endthread
.see end
.cp 15
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <process.h>
#include <dos.h>

#if defined(__386__)
  #define FAR
  #define STACK_SIZE    8192
#else
  #define FAR           __far
  #define STACK_SIZE    4096
#endif
.exmp break
static volatile int     WaitForThread;

void FAR child( void FAR *parm )
  {
    char * FAR *argv = (char * FAR *) parm;
    int  i;

    printf( "Child thread ID = %x\n", *_threadid );
    for( i = 0; argv[i]; i++ ) {
      printf( "argv[%d] = %s\n", i, argv[i] );
    }
    WaitForThread = 0;
    _endthread();
  }
.exmp break
void main()
  {
    char           *args[3];
#if defined(__NT__)
    unsigned long   tid;
#else
    char           *stack;
    int             tid;
#endif

    args[0] = "child";
    args[1] = "parm";
    args[2] = NULL;
    WaitForThread = 1;
#if defined(__NT__)
    tid = _beginthread( child, STACK_SIZE, args );
    printf( "Thread handle = %lx\n", tid );
#else
  #if defined(__386__)
    stack = (char *) malloc( STACK_SIZE );
  #else
    stack = (char *) _nmalloc( STACK_SIZE );
  #endif
    tid = _beginthread( child, stack, STACK_SIZE, args );
    printf( "Thread ID = %x\n", tid );
#endif
    while( WaitForThread ) {
        sleep( 0 );
    }
  }
.exmp end
.class WATCOM
.system
