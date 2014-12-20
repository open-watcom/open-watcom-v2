.func rename _wrename
.synop begin
#include <stdio.h>
int rename( const char *old, const char *new );
.ixfunc2 '&FileOp' &funcb
.if &'length(&wfunc.) ne 0 .do begin
int _wrename( const wchar_t *old, const wchar_t *new );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function causes the file whose name is indicated by the string
.arg old
to be renamed to the name given by the string
.arg new
.ct .li .
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns zero if the operation succeeds, a non-zero
value if it fails.
.im errnoref
.return end
.exmp begin
#include <stdio.h>

void main()
  {
    rename( "old.dat", "new.dat" );
  }
.exmp end
.class ISO C
.system
