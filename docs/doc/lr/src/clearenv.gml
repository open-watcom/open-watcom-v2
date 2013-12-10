.func clearenv
#include <env.h>
int clearenv( void );
.ixfunc2 '&Process' &func
.funcend
.desc begin
The &func function clears the process environment area.
No environment variables are defined immediately after a call to the
&func function.
.if '&machsys' eq 'QNX' .do begin
Note that this clears the
.kw PATH
.ct,
.kw SHELL
.ct,
.kw TERM
.ct,
.kw TERMINFO
.ct,
.kw LINES
.ct,
.kw COLUMNS
.ct,
and
.kw TZ
environment variables which may then affect the operation of other
library functions.
.do end
.el .do begin
Note that this clears the
.kw PATH
.ct,
.kw COMSPEC
.ct,
and
.kw TZ
environment variables which may then affect the operation of other
library functions.
.do end
.pp
The &func function may manipulate the value of the pointer
.kw environ
.ct .li .
.desc end
.return begin
The &func function returns zero upon successful completion.
Otherwise, it will return a non-zero value and set
.kw errno
to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term ENOMEM
Not enough memory to allocate a control structure.
.endterm
.error end
.see begin
.im seeenv clearenv
.see end
.exmp begin
.blktext begin
The following example clears the entire environment area and sets up a
new TZ environment variable.
.blktext end
.blkcode begin
#include <env.h>

void main()
  {
    clearenv();
    setenv( "TZ", "EST5EDT", 0 );
  }
.blkcode end
.exmp end
.class WATCOM
.system
