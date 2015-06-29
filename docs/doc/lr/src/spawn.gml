.func begin spawn&grpsfx Functions
.fungroup _spawn&grpsfx
.fungroup _wspawn&grpsfx
.func2 spawnl
.func2 spawnle
.func2 spawnlp
.func2 spawnlpe
.func2 spawnv
.func2 spawnve
.func2 spawnvp
.func2 spawnvpe
.func2 _spawnl
.func2 _spawnle
.func2 _spawnlp
.func2 _spawnlpe
.func2 _spawnv
.func2 _spawnve
.func2 _spawnvp
.func2 _spawnvpe
.func2 _wspawnl
.func2 _wspawnle
.func2 _wspawnlp
.func2 _wspawnlpe
.func2 _wspawnv
.func2 _wspawnve
.func2 _wspawnvp
.func2 _wspawnvpe
.func end
.synop begin
#include <process.h>
int spawnl(   mode, path, arg0, arg1..., argn, NULL );
int spawnle(  mode, path, arg0, arg1..., argn, NULL, envp);
int spawnlp(  mode, file, arg0, arg1..., argn, NULL );
int spawnlpe( mode, file, arg0, arg1..., argn, NULL, envp);
int spawnv(   mode, path, argv );
int spawnve(  mode, path, argv, envp );
int spawnvp(  mode, file, argv );
int spawnvpe( mode, file, argv, envp );
int _spawnl(   mode, path, arg0, arg1..., argn, NULL );
int _spawnle(  mode, path, arg0, arg1..., argn, NULL, envp);
int _spawnlp(  mode, file, arg0, arg1..., argn, NULL );
int _spawnlpe( mode, file, arg0, arg1..., argn, NULL, envp);
int _spawnv(   mode, path, argv );
int _spawnve(  mode, path, argv, envp );
int _spawnvp(  mode, file, argv );
int _spawnvpe( mode, file, argv, envp );
  int         mode;             /* mode for parent      */
  const char *path;             /* file name incl. path */
  const char *file;             /* file name            */
  const char *arg0, ..., *argn; /* arguments            */
  const char *const argv[];     /* array of arguments   */
  const char *const envp[];     /* environment strings  */
.ixfunc2 '&Process' spawnl
.ixfunc2 '&Process' spawnle
.ixfunc2 '&Process' spawnlp
.ixfunc2 '&Process' spawnlpe
.ixfunc2 '&Process' spawnv
.ixfunc2 '&Process' spawnve
.ixfunc2 '&Process' spawnvp
.ixfunc2 '&Process' spawnvpe
.ixfunc2 '&Process' _spawnl
.ixfunc2 '&Process' _spawnle
.ixfunc2 '&Process' _spawnlp
.ixfunc2 '&Process' _spawnlpe
.ixfunc2 '&Process' _spawnv
.ixfunc2 '&Process' _spawnve
.ixfunc2 '&Process' _spawnvp
.ixfunc2 '&Process' _spawnvpe
.if &'length(&wfunc.) ne 0 .do begin
.sr wfunc=_wspawn
int _wspawnl(   mode, path, arg0, arg1..., argn, NULL );
int _wspawnle(  mode, path, arg0, arg1..., argn, NULL, envp);
int _wspawnlp(  mode, file, arg0, arg1..., argn, NULL );
int _wspawnlpe( mode, file, arg0, arg1..., argn, NULL, envp);
int _wspawnv(   mode, path, argv );
int _wspawnve(  mode, path, argv, envp );
int _wspawnvp(  mode, file, argv );
int _wspawnvpe( mode, file, argv, envp );
  int            mode;             /* mode for parent      */
  const wchar_t *path;             /* file name incl. path */
  const wchar_t *file;             /* file name            */
  const wchar_t *arg0, ..., *argn; /* arguments            */
  const wchar_t *const argv[];     /* array of arguments   */
  const wchar_t *const envp[];     /* environment strings  */
.ixfunc2 '&Process' _wspawnl
.ixfunc2 '&Process' _wspawnle
.ixfunc2 '&Process' _wspawnlp
.ixfunc2 '&Process' _wspawnlpe
.ixfunc2 '&Process' _wspawnv
.ixfunc2 '&Process' _wspawnve
.ixfunc2 '&Process' _wspawnvp
.ixfunc2 '&Process' _wspawnvpe
.ixfunc2 '&Wide' _wspawnl
.ixfunc2 '&Wide' _wspawnle
.ixfunc2 '&Wide' _wspawnlp
.ixfunc2 '&Wide' _wspawnlpe
.ixfunc2 '&Wide' _wspawnv
.ixfunc2 '&Wide' _wspawnve
.ixfunc2 '&Wide' _wspawnvp
.ixfunc2 '&Wide' _wspawnvpe
.do end
.synop end
.desc begin
The
.idbold &function.
functions create and execute a new child process, named by
.arg pgm
.ct .li .
The value of
.arg mode
determines how the program is loaded and how the invoking
program will behave after the invoked program is initiated:
.begterm 14
.termhd1 Mode
.termhd2 Meaning
.term P_WAIT
The invoked program is loaded into available memory, is executed, and
then the original program resumes execution.
.if '&machsys' ne 'QNX' .do begin
This option is supported under DOS, OS/2, Win32 and QNX.
.do end
.term P_NOWAIT
Causes the current program to execute concurrently with the new child process.
.if '&machsys' ne 'QNX' .do begin
This option is supported under OS/2, Win32 and QNX.
.do end
.term P_NOWAITO
Causes the current program to execute concurrently with the new child process.
.if '&machsys' ne 'QNX' .do begin
This option is supported under OS/2, Win32 and QNX.
.do end
.if '&machsys' eq 'QNX' .do begin
The
.kw wait
function cannot be used to obtain the exit code.
.do end
.el .do begin
The
.kw wait
and
.kw cwait
functions cannot be used to obtain the exit code.
.do end
.term P_OVERLAY
The invoked program replaces the original program in memory and is
executed.
No return is made to the original program.
.if '&machsys' ne 'QNX' .do begin
This option is supported under DOS (16-bit only), OS/2, Win32, and
QNX.
.do end
This is equivalent to calling the appropriate
.kw exec&grpsfx
function.
.term P_DETACH
Launches the child process in the background without access to the keyboard or
console.
.if '&machsys' ne 'QNX' .do begin
This option is supported under only Windows NT.
.do end
.endterm
.*==========================================
.im execcom spawn
.*==========================================
.np
Arguments are passed to the child process by supplying one or more
pointers to character strings as arguments in the
.idbold &function.
call.
.if '&machsys' ne 'QNX' .do begin
These character strings are concatenated with spaces inserted to separate
the arguments to form one argument string for the child process.
The length of this concatenated string must not exceed 128 bytes for
DOS systems.
.do end
.np
The arguments may be passed as a list of arguments (
.ct .kw spawnl
.ct,
.kw spawnle
.ct,
.kw spawnlp
and
.kw spawnlpe
.ct ) or as a vector of pointers (
.ct .kw spawnv
.ct,
.kw spawnve
.ct,
.kw spawnvp
.ct,
and
.kw spawnvpe
.ct ).
At least one argument,
.arg arg0
or
.arg argv[0]
.ct , must be passed to the child process.
By convention, this first argument is a pointer to the name of the
program.
.np
If the arguments are passed as a list, there must be a
.mono NULL
pointer to mark the end of the argument list.
Similarly, if a pointer to an argument vector is passed, the argument
vector must be terminated by a
.mono NULL
pointer.
.*==========================================
.np
The environment for the invoked program is inherited from the parent
process when you use the
.kw spawnl
.ct,
.kw spawnlp
.ct,
.kw spawnv
and
.kw spawnvp
functions.
The
.kw spawnle
.ct,
.kw spawnlpe
.ct,
.kw spawnve
and
.kw spawnvpe
functions allow a different environment to be passed to the child
process through the
.arg envp
argument.
The argument
.arg envp
is a pointer to an array of character pointers, each of which points
to a string defining an environment variable.
The array is terminated with a
.mono NULL
pointer.
Each pointer locates a character string of the form
.millust begin
    variable=value
.millust end
.pc
that is used to define an environment variable.
If the value of
.arg envp
is
.mono NULL
.ct , then the child process inherits the environment of the parent process.
.np
The environment is the collection of environment variables whose
values that have been defined with the
.if '&machsys' eq 'QNX' .do begin
.qnxcmd export
or by the successful execution of the
.kw putenv
or
.kw setenv
functions.
.do end
.el .do begin
.doscmd SET
or by the successful execution of the
.kw putenv
function.
.do end
A program may read these values with the
.kw getenv
function.
.np
The following example invokes "myprog" as if
.mono myprog ARG1 ARG2
had been entered as a command to &machsys..
.millust begin
spawnl( P_WAIT, "myprog",
        "myprog", "ARG1", "ARG2", NULL );
.millust end
.pc
.if '&machsys' eq 'QNX' .do begin
The program will be found if "myprog"
.do end
.el .do begin
The program will be found if one of
"myprog.",
"myprog.com",
or
"myprog.exe"
.do end
is found in the current working directory.
.np
The following example includes a new environment for "myprog".
.millust begin
char *env_list[] = { "SOURCE=MYDATA",
                     "TARGET=OUTPUT",
                     "lines=65",
                     NULL
                    };

spawnle( P_WAIT, "myprog",
        "myprog", "ARG1", "ARG2", NULL,
         env_list );
.millust end
.np
The environment for the invoked program will consist of the three
environment variables
.mono SOURCE, TARGET
and
.mono lines.
.np
The following example is another variation on the first example.
.millust begin
char *arg_list[] = { "myprog", "ARG1", "ARG2", NULL };

spawnv( P_WAIT, "myprog", arg_list );
.millust end
.*==========================================
.im widefun5
.*==========================================
.desc end
.return begin
When the value of
.arg mode
is:
.begterm 14
.termhd1 Mode
.termhd2 Meaning
.term P_WAIT
then the return value from
.idbold &function.
is the exit status of the child process.
.term P_NOWAIT
then the return value from
.idbold &function.
is the process id (or process handle
under Win32) of the child process.
To obtain the exit code for a process spawned with
.kw P_NOWAIT
.ct,
you must call the
.kw wait
(under OS/2 or QNX)
.if '&machsys' ne 'QNX' .do begin
or
.kw cwait
(under OS/2 or Win32)
.do end
function specifying the process id/handle.
If the child process terminated normally, then the low order byte of
the returned status word will be set to 0, and the high order byte
will contain the low order byte of the return code that the child
process passed to the
.kw DOSEXIT
function.
.term P_NOWAITO
then the return value from
.idbold &function.
is the process id of the child process.
The exit code cannot be obtained for a process spawned with
.kw P_NOWAITO
.ct .li .
.term P_DETACH
then the return value from
.idbold &function.
is zero (0) if successful.
.endterm
.np
When an error is detected while invoking the indicated program, &function
returns &minus.1 and
.kw errno
is set to indicate the error.
.return end
.error begin
.if '&machsys' eq 'QNX' .do begin
See the
.kw qnx_spawn
function for a description of possible
.kw errno
values.
.do end
.el .do begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term E2BIG
The argument list exceeds 128 bytes, or the space required for the
environment information exceeds 32K.
.term EINVAL
The
.arg mode
argument is invalid.
.term ENOENT
Path or file not found
.term ENOMEM
Not enough memory is available to execute the child process.
.endterm
.do end
.error end
.*==========================================
.see begin
.seelist abort atexit cwait exec&grpsfx exit _Exit _exit
.seelist getcmd getenv main putenv qnx_spawn qnx_spawn_options
.seelist system wait
.see end
.cp 8
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <string.h>

void main()
{
    int     process_id;
#if defined(__OS2__) || defined(__NT__)
    int     status, rc;
#endif
.exmp break
    process_id = spawnl( P_NOWAIT, "child.exe",
                         "child", "5", NULL );
    if( process_id == -1 ) {
        printf( "spawn failed - %s\n", strerror( errno ) );
        exit( EXIT_FAILURE );
    }
    printf( "Process id = %d\n", process_id );
.exmp break
#if defined(__OS2__) || defined(__NT__)
    rc = cwait( &status, process_id, WAIT_CHILD );
    if( rc == -1 ) {
        printf( "wait failed - %s\n", strerror( errno ) );
    } else {
        printf( "wait succeeded - %x\n", status );
        switch( status & 0xff ) {
        case 0:
            printf( "Normal termination exit code = %d\n",
                    status >> 8 );
            break;
        case 1:
            printf( "Hard-error abort\n" );
            break;
        case 2:
            printf( "Trap operation\n" );
            break;
        case 3:
            printf( "SIGTERM signal not intercepted\n" );
            break;
        default:
            printf( "Bogus return status\n" );
        }
    }
#endif
    printf( "spawn completed\n" );
}
.exmp break
/*
[child.c]
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

void main( int argc, char *argv[] )
{
    int delay;

    if( argc <= 1 )
        exit( EXIT_FAILURE );
    delay = atoi( argv[1] );
    printf( "I am a child going to sleep "
            "for %d seconds\n", delay );
    sleep( delay );
    printf( "I am a child awakening\n" );
    exit( 123 );

}
*/
.exmp end
.ansiname _spawn&grpsfx
.class WATCOM
.system
