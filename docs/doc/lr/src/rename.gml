.func rename _wrename _urename
#include <stdio.h>
int rename( const char *old, const char *new );
.ixfunc2 '&FileOp' &func
.if &'length(&wfunc.) ne 0 .do begin
int _wrename( const wchar_t *old, const wchar_t *new );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _urename( const wchar_t *old, const wchar_t *new );
.ixfunc2 '&FileOp' &ufunc
.do end
.funcend
.desc begin
The &func function causes the file whose name is indicated by the string
.arg old
to be renamed to the name given by the string
.arg new
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function is identical to &func except that it accepts
wide-character string arguments.
.do end
.if &'length(&ufunc.) ne 0 .do begin
The &ufunc Unicode function is identical to &func except that it
accepts Unicode string arguments.
.do end
.desc end
.return begin
The &func function returns zero if the operation succeeds, a non-zero
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
.class ANSI
.system
