.func putc putwc
.synop begin
.funcw putwc
#include <stdio.h>
int putc( int c, FILE *fp );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
wint_t putwc( wint_t c, FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function is equivalent to
.kw fputc
.ct,
except it may be implemented as a macro.
The
.id &func.
function writes the character specified by the argument
.arg c
to the output stream designated by
.arg fp
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it converts the
wide character specified by
.arg c
to a multibyte character and writes it to the output stream.
.do end
.desc end
.return begin
The
.id &func.
function returns the character written or, if a write error
occurs, the error indicator is set and
.id &func.
returns
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function returns the wide character written or, if a write
error occurs, the error indicator is set and
.id &wfunc.
returns
.kw WEOF
.ct .li .
.do end
.np
.im errnoref
.return end
.see begin
.seelist &function. fopen fputc fputchar fputs putc putchar puts ferror
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
      while( (c = fgetc( fp )) != EOF )
          putc( c, stdout );
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
