.func getcmd
.synop begin
#include <process.h>
char *getcmd( char *cmd_line );
.ixfunc2 '&Process' &funcb
.ixfunc2 '&Direct' &funcb
.synop end
.desc begin
The
.id &funcb.
function causes the command line information, with the program
name removed, to be copied to
.arg cmd_line
.ct .li .
The information is terminated with a
.mono '\0'
character.
.if '&machsys' eq 'QNX' .do begin
This provides a method of obtaining the original parameters to a program
as a single string of text.
.do end
.el .do begin
This provides a method of obtaining the original parameters to a program
unchanged (with the white space intact).
.do end
.np
This information can also be obtained by examining the vector of program
parameters passed to the main function in the program.
.desc end
.return begin
The address of the target
.arg cmd_line
is returned.
.return end
.see begin
.im seeproc
.see end
.exmp begin
.blktext begin
Suppose a program were invoked with the command line
.millust begin
myprog arg-1 ( my   stuff ) here
.millust end
where that program contains
.blktext end
.blkcode begin
#include <stdio.h>
#include <process.h>

void main()
  {
    char cmds[128];
.exmp break
    printf( "%s\n", getcmd( cmds ) );
  }
.blkcode end
.exmp output
.if '&machsys' eq 'QNX' .do begin
arg-1 ( my stuff ) here
.do end
.el .do begin
  arg-1 ( my   stuff ) here
.do end
.exmp end
.class WATCOM
.system
