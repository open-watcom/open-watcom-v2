.func getc getwc
.synop begin
.funcw getwc
#include <stdio.h>
int getc( FILE *fp );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
wint_t getwc( FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function gets the next character from the file designated by
.arg fp
.ct .li .
The character is returned as an
.id int
value.
The &func function is equivalent to
.kw fgetc
.ct,
except that it may be implemented as a macro.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it gets the next
multibyte character (if present) from the input stream pointed to by
.arg fp
and converts it to a wide character.
.do end
.desc end
.return begin
The &func function returns the next character from the input stream
pointed to by
.arg fp
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
.arg fp
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
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( (c = getc( fp )) != EOF )
        putchar(c);
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
