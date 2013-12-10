.func fcloseall
#include <stdio.h>
int fcloseall( void );
.ixfunc2 '&StrIo' &func
.funcend
.desc begin
The &func function closes all open stream files, except
.if '&machsys' eq 'QNX' .do begin
.kw stdin
.ct,
.kw stdout
.ct,
and
.kw stderr
.ct .li .
.do end
.el .do begin
.kw stdin
.ct,
.kw stdout
.ct,
.kw stderr
.ct,
.kw stdaux
.ct,
and
.kw stdprn
.ct .li .
.do end
This includes streams created (and not yet closed) by
.kw fdopen
.ct,
.kw fopen
and
.kw freopen
.ct .li .
.if '&machsys' eq 'DOS' .do begin
The
.us stdaux
and
.us stdprn
files are not available for some Windows platforms.
.do end
.desc end
.return begin
The &func function returns the number of streams that were closed if
no errors were encountered.
When an error occurs,
.kw EOF
is returned.
.return end
.see begin
.seelist &function. fclose fcloseall fdopen fopen freopen _fsopen
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    printf( "The number of files closed is %d\n",
            fcloseall() );
  }
.exmp end
.class WATCOM
.system
