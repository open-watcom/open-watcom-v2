.func getchar getwchar
.synop begin
#include <stdio.h>
int getchar( void );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wint_t getwchar( void );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function is equivalent to
.kw getc
with the argument
.kw stdin
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is similar to
.id &funcb.
except that it is equivalent
to
.kw getwc
with the argument
.kw stdin
.ct .li .
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
    while( (c = getchar()) != EOF )
      putchar(c);
    fclose( fp );
  }
.exmp end
.class ISO C
.system
