.func getenv_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
errno_t getenv_s( size_t * restrict len,
                  char * restrict value,
		  rsize_t maxsize,
                  const char * restrict name );
.synop end
.*
.rtconst begin
.arg name
shall not be a null pointer.
.arg maxsize
shall neither be equal to zero nor be greater than
.kw RSIZE_MAX
.ct .li .
If
.arg maxsize
is not equal to zero, then
.arg value
shall not be a null pointer.
.np
If there is a runtime-constraint violation, the integer pointed to by
.arg len
(if
.arg len
is not null) is set to zero, and the environment list is not searched.
.rtconst end
.*
.desc begin
The &func function searches the environment list for an entry matching
the string pointed to by
.arg name
.ct .li .
.np
If that entry is found, &func performs the following actions. If
.arg len
is not a null pointer, the length of the string associated with the matched
entry is stored in the integer pointed to by
.arg len
.ct .li .
If the length of the associated string is less than
.arg maxsize
.ct , then the associated string is copied to the array pointed to by
.arg value
.ct .li .
.np
If that entry is not found, &func performs the following actions. If
.arg len
is not a null pointer, zero is stored in the integer pointed to by
.arg len
.ct .li .
If
.arg maxsize
is greater than zero, then
.arg value[0]
is set to the null character.
.np
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
.desc end
.*
.return begin
The &func function returns zero if the environment string specified by
.arg name
was found and successfully stored in the buffer pointed to by
.arg value
.ct .li .
Otherwise, a non-zero value is returned.
.return end
.*
.see begin
.im seeenv getenv_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
#include <stdio.h>
.exmp break
void main( void )
{
    char    buffer[128];
    size_t  len;
.exmp break
    if( getenv_s( &len, buffer, sizeof( buffer ), "INCLUDE" ) == 0 )
        printf( "INCLUDE=%s\n", buffer );
}
.exmp end
.*
.class TR 24731
.system
