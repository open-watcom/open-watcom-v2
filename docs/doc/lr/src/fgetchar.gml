.func fgetchar _fgetchar _fgetwchar
.synop begin
#include <stdio.h>
int fgetchar( void );
.ixfunc2 '&StrIo' &funcb
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
The
.id &funcb.
function is equivalent to
.kw fgetc
with the argument
.kw stdin
.ct .li .
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to
.id &funcb.
.
Use
.id &_func.
for ISO C naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it gets the next
multibyte character (if present) from the input stream pointed to by
.kw stdin
and converts it to a wide character.
.do end
.desc end
.return begin
The
.id &funcb.
function returns the next character from the input stream
pointed to by
.kw stdin
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
.id &funcb.
returns
.kw EOF
.ct .li .
If a read error occurs, the error indicator is set and
.id &funcb.
returns
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function returns the next wide character from the input
stream pointed to by
.kw stdin
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
.id &wfunc.
returns
.kw WEOF
.ct .li .
If a read error occurs, the error indicator is set and
.id &wfunc.
returns
.kw WEOF
.ct .li .
If an encoding error occurs,
.kw errno
is set to
.kw EILSEQ
and
.id &wfunc.
returns
.kw WEOF
.ct .li .
.do end
.np
.im errnoref
.return end
.see begin
.seelist fgetc fgetchar fgets fopen getc getchar gets ungetc
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
