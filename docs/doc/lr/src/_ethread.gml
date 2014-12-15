.func begin _endthread
.func2 _endthreadex
.func end
.synop begin
#include <process.h>
void _endthread(void);
.ixfunc2 '&OS2Func' &funcb
.ixfunc2 '&NTFunc' &funcb
void _endthreadex( unsigned retval );
.ixfunc2 '&NTFunc' &funcb.ex
.synop end
.desc begin
The
.id &funcb.
function is used to terminate a thread created by
.kw _beginthread
.ct .li .
For each operating environment under which
.id &funcb.
is supported,
the
.id &funcb.
function uses the appropriate system call to end the current
thread of execution.
.np
The
.kw _endthreadex
function is used to terminate a thread created by
.kw _beginthreadex
.ct .li .
The thread exit code
.arg retval
must be specified.
.desc end
.return begin
The
.id &funcb.
function does not return any value.
.return end
.see begin
.seelist _beginthread _endthread
.see end
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
