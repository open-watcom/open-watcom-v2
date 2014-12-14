.func system _wsystem
.synop begin
#include <stdlib.h>
int system( const char *command );
.ixfunc2 '&Process' &func
.if &'length(&wfunc.) ne 0 .do begin
int _wsystem( const wchar_t *command );
.ixfunc2 '&Process' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
.if '&machsys' eq 'QNX' .do begin
.sr shell = 'shell'
If the value of
.arg command
is
.mono NULL,
then the
.id &func.
function determines whether or not a &shell is
present.
On a POSIX 1003.2 system (e.g., QNX), the shell is always assumed
present and &func.(NULL) always returns a non-zero value.
.do end
.el .do begin
.sr shell = 'command processor'
If the value of
.arg command
is
.mono NULL,
then the
.id &func.
function determines whether or not a &shell is
present ("COMMAND.COM" in DOS and Windows 95/98
or "CMD.EXE" in OS/2 and Windows NT/2000).
.do end
.np
Otherwise, the
.id &func.
function invokes a copy of the &shell, and passes
the string
.arg command
to it for processing.
.if '&machsys' eq 'QNX' .do begin
This function uses
.kw spawnlp
to load a copy of the &shell..
.do end
.el .do begin
This function uses
.kw spawnl
to load a copy of the &shell identified by the
.kw COMSPEC
environment variable.
.do end
.if '&machsys' eq 'QNX' .do begin
.np
Note that the shell used is always
.mono /bin/sh,
regardless of the setting of the
.kw SHELL
environment variable.
This is so because applications may rely on features of the standard
shell and may fail as a result of running a different shell.
.do end
.np
This means that any command that can be entered to &machsys can be
executed, including programs, &machsys commands and
.if '&machsys' eq 'QNX' .do begin
shell scripts.
.do end
.el .do begin
batch files.
.do end
The
.kw exec...
and
.kw spawn...
functions can only cause programs to be executed.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it accepts a
wide-character string argument.
.do end
.desc end
.return begin
If the value of
.arg command
is
.mono NULL,
then the
.id &func.
function returns zero if the &shell is not present,
a non-zero value if the &shell is present.
.if '&machsys' eq 'QNX' .do begin
This implementation always returns a non-zero value.
.do end
.el .do begin
Note that Microsoft Windows 3.x does not support a command shell and
so the
.id &func.
function always returns zero when
.arg command
is
.mono NULL.
.do end
.np
Otherwise, the
.id &func.
function returns the result of invoking a copy of
the &shell..
.if '&machsys' eq 'QNX' .do begin
A -1 is returned if the &shell could not be loaded; otherwise, the
status of the specified command is returned.
Assume that "status" is the value returned by &func..
If
.mono WEXITSTATUS( status ) == 255,
this indicates that the specified command could not be run.
.mono WEXITSTATUS
is defined in
.hdrfile sys/wait.h
.do end
.el .do begin
A non-zero value is returned if the &shell could not be loaded;
otherwise, zero is returned.
.do end
.im errnoref
.return end
.see begin
.im seeproc
.see end
.cp 12
.exmp begin
.if '&machsys' eq 'QNX' .do begin
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

void main()
  {
    int rc;
.exmp break
    rc = system( "ls" );
    if( rc == -1 ) {
      printf( "shell could not be run\n" );
    } else {
      printf( "result of running command is %d\n",
              WEXITSTATUS( rc ) );
    }
  }
.do end
.el .do begin
#include <stdlib.h>
#include <stdio.h>

void main()
  {
    int rc;
.exmp break
    rc = system( "dir" );
    if( rc != 0 ) {
      printf( "shell could not be run\n" );
    }
  }
.do end
.exmp end
.class ANSI, POSIX 1003.2
.system
