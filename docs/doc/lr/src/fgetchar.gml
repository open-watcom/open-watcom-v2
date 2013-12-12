.func fgetchar _fgetchar _fgetwchar
.synop begin
.funcw _fgetwchar
#include <stdio.h>
int fgetchar( void );
.ixfunc2 '&StrIo' &func
.if &'length(&_func.) ne 0 .do begin
int _fgetchar( void );
.ixfunc2 '&StrIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wint_t _fgetwchar( void );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function is equivalent to
.kw fgetc
with the argument
.kw stdin
.ct .li .
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it gets the next
multibyte character (if present) from the input stream pointed to by
.kw stdin
and converts it to a wide character.
.do end
.desc end
.return begin
The &func function returns the next character from the input stream
pointed to by
.kw stdin
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
&func returns
.kw EOF
.ct .li .
If a read error occurs, the error indicator is set and
&func returns
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the next wide character from the input
stream pointed to by
.kw stdin
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
&wfunc returns
.kw WEOF
.ct .li .
If a read error occurs, the error indicator is set and &wfunc returns
.kw WEOF
.ct .li .
If an encoding error occurs,
.kw errno
is set to
.kw EILSEQ
and &wfunc returns
.kw WEOF
.ct .li .
.do end
.np
.im errnoref
.return end
.see begin
.seelist &function. fgetc fgetchar fgets fopen getc getchar gets ungetc
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    int c;
.exmp break
    fp = freopen( "file", "r", stdin );
    if( fp != NULL ) {
      while( (c = fgetchar()) != EOF )
        fputchar(c);
      fclose( fp );
    }
  }
.exmp end
.class WATCOM
.system
