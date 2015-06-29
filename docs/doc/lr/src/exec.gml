.func begin exec&grpsfx Functions
.fungroup _exec&grpsfx
.fungroup _wexec&grpsfx
.func2 execl
.func2 execle
.func2 execlp
.func2 execlpe
.func2 execv
.func2 execve
.func2 execvp
.func2 execvpe
.func2 _execl
.func2 _execle
.func2 _execlp
.func2 _execlpe
.func2 _execv
.func2 _execve
.func2 _execvp
.func2 _execvpe
.func2 _wexecl
.func2 _wexecle
.func2 _wexeclp
.func2 _wexeclpe
.func2 _wexecv
.func2 _wexecve
.func2 _wexecvp
.func2 _wexecvpe
.func end
.synop begin
.* .synop begin
#include <process.h>
int execl(   path, arg0, arg1..., argn, NULL );
int execle(  path, arg0, arg1..., argn, NULL, envp );
int execlp(  file, arg0, arg1..., argn, NULL );
int execlpe( file, arg0, arg1..., argn, NULL, envp );
int execv(   path, argv );
int execve(  path, argv, envp );
int execvp(  file, argv );
int execvpe( file, argv, envp );
int _execl(   path, arg0, arg1..., argn, NULL );
int _execle(  path, arg0, arg1..., argn, NULL, envp );
int _execlp(  file, arg0, arg1..., argn, NULL );
int _execlpe( file, arg0, arg1..., argn, NULL, envp );
int _execv(   path, argv );
int _execve(  path, argv, envp );
int _execvp(  file, argv );
int _execvpe( file, argv, envp );
  const char *path;             /* file name incl. path */
  const char *file;             /* file name            */
  const char *arg0, ..., *argn; /* arguments            */
  const char *const argv[];     /* array of arguments   */
  const char *const envp[];     /* environment strings  */
.ixfunc2 '&Process' execl
.ixfunc2 '&Process' execle
.ixfunc2 '&Process' execlp
.ixfunc2 '&Process' execlpe
.ixfunc2 '&Process' execv
.ixfunc2 '&Process' execve
.ixfunc2 '&Process' execvp
.ixfunc2 '&Process' execvpe
.ixfunc2 '&Process' _execl
.ixfunc2 '&Process' _execle
.ixfunc2 '&Process' _execlp
.ixfunc2 '&Process' _execlpe
.ixfunc2 '&Process' _execv
.ixfunc2 '&Process' _execve
.ixfunc2 '&Process' _execvp
.ixfunc2 '&Process' _execvpe
.if &'length(&wfunc.) ne 0 .do begin
.sr wfunc=_wexec
int _wexecl(   path, arg0, arg1..., argn, NULL );
int _wexecle(  path, arg0, arg1..., argn, NULL, envp );
int _wexeclp(  file, arg0, arg1..., argn, NULL );
int _wexeclpe( file, arg0, arg1..., argn, NULL, envp );
int _wexecv(   path, argv );
int _wexecve(  path, argv, envp );
int _wexecvp(  file, argv );
int _wexecvpe( file, argv, envp );
  const wchar_t *path;            /* file name incl. path */
  const wchar_t *file;            /* file name            */
  const wchar_t *arg0, ..., *argn;/* arguments            */
  const wchar_t *const argv[];    /* array of arguments   */
  const wchar_t *const envp[];    /* environment strings  */
.ixfunc2 '&Process' _wexecl
.ixfunc2 '&Process' _wexecle
.ixfunc2 '&Process' _wexeclp
.ixfunc2 '&Process' _wexeclpe
.ixfunc2 '&Process' _wexecv
.ixfunc2 '&Process' _wexecve
.ixfunc2 '&Process' _wexecvp
.ixfunc2 '&Process' _wexecvpe
.ixfunc2 '&Wide' _wexecl
.ixfunc2 '&Wide' _wexecle
.ixfunc2 '&Wide' _wexeclp
.ixfunc2 '&Wide' _wexeclpe
.ixfunc2 '&Wide' _wexecv
.ixfunc2 '&Wide' _wexecve
.ixfunc2 '&Wide' _wexecvp
.ixfunc2 '&Wide' _wexecvpe
.do end
.synop end
.desc begin
The
.idbold &function.
functions load and execute a new child process, named by
.arg path
or
.arg file
.ct .li .
If the child process is successfully loaded, it replaces the current
process in memory.
No return is made to the original program.
.*==========================================
.im execcom exec
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
.ct .kw execl
.ct,
.kw execle
.ct,
.kw execlp
.ct,
and
.kw execlpe
.ct )
or as a vector of pointers (
.ct .kw execv
.ct,
.kw execve
.ct,
.kw execvp
.ct , and
.kw execvpe
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
.kw execl
.ct ,
.kw execlp
.ct ,
.kw execv
.ct , and
.kw execvp
functions.
The
.kw execle
.ct ,
.kw execlpe
.ct ,
.kw execve
.ct , and
.kw execvpe
functions allow a different environment to be passed to the child process
through the
.arg envp
argument.
The argument
.arg envp
is a pointer to an array of character pointers, each of which points to
a string defining an environment variable.
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
values have been defined with the
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
.*==========================================
.np
The
.kw execvpe
and
.kw execlpe
functions are extensions to POSIX 1003.1.
.*==========================================
.im widefun5
.*==========================================
.desc end
.return begin
When the invoked program is successfully initiated, no return occurs.
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
.term EACCES
The specified file has a locking or sharing violation.
.term EMFILE
Too many files open
.term ENOENT
Path or file not found
.term ENOMEM
Not enough memory is available to execute the child process.
.endterm
.do end
.error end
.*==========================================
.see begin
.seelist abort atexit exit _Exit _exit
.seelist getcmd getenv main putenv qnx_spawn qnx_spawn_options
.seelist spawn&grpsfx
.seelist system
.see end
.exmp begin
#include <stddef.h>
#include <process.h>

execl( "myprog",
        "myprog", "ARG1", "ARG2", NULL );
.blktext begin
The preceding invokes "myprog" as if
.blktext end
.blkcode begin
    myprog ARG1 ARG2
.blkcode end
.blktext begin
had been entered as a command to &machsys..
.if '&machsys' eq 'QNX' .do begin
The program will be found if "myprog"
.do end
.el .do begin
The program will be found if one of
.blktext end
.blkcode begin
    myprog.
    myprog.com
    myprog.exe
.blkcode end
.blktext begin
.do end
is found in the current working directory.
.blktext end
.blkcode begin
#include <stddef.h>
#include <process.h>

char *env_list[] = { "SOURCE=MYDATA",
                     "TARGET=OUTPUT",
                     "lines=65",
                     NULL
                    };

execle( "myprog",
        "myprog", "ARG1", "ARG2", NULL,
         env_list );
.blkcode end
.blktext begin
The preceding invokes "myprog" as if
.blktext end
.blkcode begin
    myprog ARG1 ARG2
.blkcode end
.blktext begin
had been entered as a command to &machsys..
.if '&machsys' eq 'QNX' .do begin
The program will be found if "myprog"
.do end
.el .do begin
The program will be found if one of
.blktext end
.blkcode begin
    myprog.
    myprog.com
    myprog.exe
.blkcode end
.blktext begin
.do end
is found in the current working directory.
The &machsys environment for the invoked program will consist of the
three environment variables
.mono SOURCE, TARGET
and
.mono lines.
.blktext end
.blkcode begin
#include <stddef.h>
#include <process.h>

char *arg_list[] = { "myprog", "ARG1", "ARG2", NULL };

execv( "myprog", arg_list );
.blkcode end
.blktext begin
The preceding invokes "myprog" as if
.blktext end
.blkcode begin
    myprog ARG1 ARG2
.blkcode end
.blktext begin
had been entered as a command to &machsys..
.if '&machsys' eq 'QNX' .do begin
The program will be found if "myprog"
.do end
.el .do begin
The program will be found if one of
.blktext end
.blkcode begin
    myprog.
    myprog.com
    myprog.exe
.blkcode end
.blktext begin
.do end
is found in the current working directory.
.blktext end
.exmp end
.ansiname _exec&grpsfx
.class POSIX 1003.1 with extensions
.system
