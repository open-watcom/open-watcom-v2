.func putenv _putenv _wputenv
.synop begin
#include <stdlib.h>
int putenv( const char *env_name );
.ixfunc2 '&Process' &funcb
.if &'length(&_func.) ne 0 .do begin
int _putenv( const char *env_name );
.ixfunc2 '&Process' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wputenv( const wchar_t *env_name );
.ixfunc2 '&Process' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
.ix 'environment'
The environment list consists of a number of environment names,
each of which has a value associated with it.
Entries can be added to the environment list with the
.if '&machsys' eq 'QNX' .do begin
.qnxcmd export
.do end
.el .do begin
.doscmd set
.do end
or with the
.id &funcb.
function.
All entries in the environment list can be displayed by using the
.if '&machsys' eq 'QNX' .do begin
.qnxcmd export
.do end
.el .do begin
.doscmd set
.do end
with no arguments.
A program can obtain the value for an environment variable by using
the
.kw getenv
function.
.np
When the value of
.arg env_name
has the format
.millust begin
    env_name=value
.millust end
.pc
an environment name and its value is added to the environment list.
When the value of
.arg env_name
has the format
.millust begin
    env_name=
.millust end
.pc
the environment name and value is removed from the environment list.
.np
.if '&machsys' eq 'QNX' .do begin
The matching is case-sensitive; all lowercase letters are treated
as different from uppercase letters.
.do end
.el .do begin
The matching is case-insensitive; all lowercase letters are treated
as if they were in upper case.
.do end
.np
The space into which environment names and their values are placed
is limited.
Consequently, the
.id &funcb.
function can fail when there is insufficient
space remaining to store an additional value.
.im ansiconf
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is a wide-character version of
.id &funcb.
the
.arg env_name
argument to
.id &wfunc.
is a wide-character string.
.np
.id &funcb.
and
.id &wfunc.
affect only the environment that is local to the
current process; you cannot use them to modify the command-level
environment. That is, these functions operate only on data structures
accessible to the run-time library and not on the environment
"segment" created for a process by the operating system. When the
current process terminates, the environment reverts to the level of
the calling process (in most cases, the operating-system level).
However, the modified environment can be passed to any new processes
created by _spawn, _exec, or system, and these new processes get any
new items added by
.id &funcb.
and &wfunc..
.np
With regard to environment entries, observe the following cautions:
.begbull
.bull
Do not change an environment entry directly; instead, use &funcb
or
.id &wfunc.
to change it. To modify the return value of
.id &funcb.
or
.id &wfunc.
without affecting the environment table, use
.kw _strdup
or
.kw strcpy
to make a copy of the string.
.bull
If the argument
.arg env_name
is not a literal string, you should duplicate the string, since &funcb
does not copy the value; for example,
.millust begin
    putenv( _strdup( buffer ) );
.millust end
.bull
Never free a pointer to an environment entry, because the environment
variable will then point to freed space. A similar problem
can occur if you pass
.id &funcb.
or
.id &wfunc.
a pointer to a local
variable, then exit the function in which the variable is declared.
.endbull
:CMT. .np
:CMT. getenv and
.id &funcb.
use the global variable _environ to access the
:CMT. environment table; _wgetenv and
.id &wfunc.
use _wenviron.
.id &funcb.
and
:CMT.
.id &wfunc.
may change the value of _environ and _wenviron, thus
:CMT. invalidating the envp argument to main and the _wenvp argument to wmain.
:CMT. Therefore, it is safer to use _environ or _wenviron to access the
:CMT. environment information. For more information about the relation of
:CMT.
.id &funcb.
and
.id &wfunc.
to global variables, see _environ, _wenviron.
.do end
.np
To assign a string to a variable and place it in the environment list:
.millust begin
.if '&machsys' eq 'QNX' .do begin
    % export INCLUDE=/usr/include
.do end
.el .do begin
    C>SET INCLUDE=C:\WATCOM\H
.do end
.millust end
.np
To see what variables are in the environment list, and their current
assignments:
.millust begin
.if '&machsys' eq 'QNX' .do begin
    % export
    SHELL=ksh
    TERM=qnx
    LOGNAME=fred
    PATH=:/bin:/usr/bin
    HOME=/home/fred
    INCLUDE=/usr/include
    LIB=/usr/lib
    %
.do end
.el .do begin
    C>SET
    COMSPEC=C:\COMMAND.COM
    PATH=C:\;C:\WATCOM
    INCLUDE=C:\WATCOM\H

    C>
.do end
.millust end
.desc end
.return begin
The
.id &funcb.
function returns zero when it is successfully executed and
returns &minus.1 when it fails.
.return end
.error begin
.begterm 12
.term ENOMEM
Not enough memory to allocate a new environment string.
.endterm
.error end
.see begin
.seelist putenv clearenv getenv setenv
.see end
.exmp begin
.blktext begin
The following gets the string currently assigned to
.kw INCLUDE
and displays it, assigns a new value to it, gets and displays it,
and then removes the environment name and value.
.blktext end
.blkcode begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main()
  {
    char *path;
    path = getenv( "INCLUDE" );
    if( path != NULL )
        printf( "INCLUDE=%s\n", path );
.if '&machsys' eq 'QNX' .do begin
    if( putenv( "INCLUDE=//5/usr/include" ) != 0 )
.do end
.el .do begin
    if( putenv( "INCLUDE=mylib;yourlib" ) != 0 )
.do end
        printf( "putenv failed" );
    path = getenv( "INCLUDE" );
    if( path != NULL )
        printf( "INCLUDE=%s\n", path );
    if( putenv( "INCLUDE=" ) != 0 )
        printf( "putenv failed" );
  }
.blkcode end
.exmp output
.if '&machsys' eq 'QNX' .do begin
INCLUDE=/usr/include
INCLUDE=//5/usr/include
.do end
.el .do begin
INCLUDE=C:\WATCOM\H
INCLUDE=mylib;yourlib
.do end
.exmp end
.class POSIX 1003.1
.system
