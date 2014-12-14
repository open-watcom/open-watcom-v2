.func ungetc ungetwc
.synop begin
.funcw ungetwc
#include <stdio.h>
int ungetc( int c, FILE *fp );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
wint_t ungetwc( wint_t c, FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function pushes the character specified by
.arg c
back onto the input stream pointed to by
.arg fp
.ct .li .
This character will be returned by the next read on the stream.
The pushed-back character will be discarded if a call is made to the
.kw fflush
function or to a file positioning function (
.ct .kw fseek
.ct,
.kw fsetpos
or
.kw rewind
.ct ) before the next read operation is performed.
.np
Only one character (the most recent one) of pushback is remembered.
.np
The
.id &func.
function clears the end-of-file indicator, unless the value
of
.arg c
is
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it pushes the
wide character specified by
.arg c
back onto the input stream pointed to by
.arg fp
.ct .li .
.np
The
.id &wfunc.
function clears the end-of-file indicator, unless the value
of
.arg c
is
.kw WEOF
.ct .li .
.do end
.desc end
.return begin
The
.id &func.
function returns the character pushed back.
.return end
.see begin
.seelist fgetc fgetchar fgets fopen getc getchar gets ungetc
.see end
.exmp begin
#include <stdio.h>
#include <ctype.h>

void main()
  {
    FILE *fp;
    int c;
    long value;
.exmp break
    fp = fopen( "file", "r" );
    value = 0;
    c = fgetc( fp );
    while( isdigit(c) ) {
        value = value*10 + c - '0';
        c = fgetc( fp );
    }
    ungetc( c, fp ); /* put last character back */
    printf( "Value=%ld\n", value );
    fclose( fp );
  }
.exmp end
.class ANSI
.system
