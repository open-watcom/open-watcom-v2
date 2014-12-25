.func getenv _wgetenv
.synop begin
#include <stdlib.h>
char *getenv( const char *name );
.ixfunc2 '&Process' &funcb
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wgetenv( const wchar_t *name );
.ixfunc2 '&Process' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function searches the environment list for an entry matching
the string pointed to by
.arg name
.ct .li .
.if '&machsys' eq 'QNX' .do begin
The matching is case-sensitive; all lowercase letters are treated
as different from uppercase letters.
.do end
.el .do begin
The matching is case-insensitive; all lowercase letters are treated
as if they were in upper case.
.do end
.pp
Entries can be added to the environment list
.if '&machsys' eq 'QNX' .do begin
with the
.qnxcmd export
or with the
.kw putenv
or
.kw setenv
functions.
.do end
.el .do begin
with the
.doscmd set
or with the
.kw putenv
or
.kw setenv
functions.
.do end
.ix 'environment'
.if '&machsys' eq 'QNX' .do begin
All entries in the environment list can be displayed by using the
.qnxcmd export
with no arguments.
.do end
.el .do begin
All entries in the environment list can be displayed by using the
.doscmd set
with no arguments.
.do end
.pp
To assign a string to a variable and place it in the environment list:
.millust begin
.if '&machsys' eq 'QNX' .do begin
    % export INCLUDE=/usr/include
.do end
.el .do begin
    C>SET INCLUDE=C:\WATCOM\H
.do end
.millust end
.if '&machsys' eq 'QNX' .do begin
.pp
To see what variables are in the environment list, and their current
assignments:
.millust begin
    % export
    SHELL=ksh
    TERM=qnx
    LOGNAME=fred
    PATH=:/bin:/usr/bin
    HOME=/home/fred
    INCLUDE=/usr/include
    LIB=/usr/lib
    %
.millust end
.do end
.el .do begin
.pp
To see what variables are in the environment list, and their current
assignments:
.millust begin
    C>SET
    COMSPEC=C:\COMMAND.COM
    PATH=C:\;C:\WATCOM
    INCLUDE=C:\WATCOM\H
.millust end
.do end
.im widefun1
:CMT. .if &'length(&wfunc.) ne 0 .do begin
:CMT. The
:CMT. .kw _wenviron
:CMT. global variable is a wide-character version of
:CMT. .kw _environ
:CMT. .ct .li .
:CMT. .np
:CMT. In an MBCS program (for example, in an SBCS ASCII program),
:CMT. .kw _wenviron
:CMT. is initially NULL because the environment is composed of
:CMT. multibyte-character strings.
:CMT. Then, on the first call to
:CMT. .kw _wputenv
:CMT. .ct , or on the first call to
:CMT. .kw _wgetenv
:CMT. if an (MBCS) environment already exists, a corresponding
:CMT. wide-character string environment is created and is then
:CMT. pointed to by
:CMT. .kw _wenviron
:CMT. .ct .li .
:CMT. .do end
.desc end
.*
.return begin
The
.id &funcb.
function returns a pointer to the string assigned to the
environment variable if found, and NULL if no match was found.
Note: the value returned should be duplicated if you intend to
modify the contents of the string.
.return end
.*
.see begin
.im seeenv
.see end
.*
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main( void )
{
    char *path;
.exmp break
    path = getenv( "INCLUDE" );
    if( path != NULL )
        printf( "INCLUDE=%s\n", path );
}
.exmp end
.*
.class ISO C
.system
