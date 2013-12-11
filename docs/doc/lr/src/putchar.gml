.func putchar putwchar
.funcw putwchar
#include <stdio.h>
int putchar( int c );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wint_t putwchar( wint_t c );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function writes the character specified by the argument
.arg c
to the output stream
.kw stdout
.ct .li .
.pp
The function is equivalent to
.millust begin
fputc( c, stdout );
.millust end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it converts the
wide character specified by
.arg c
to a multibyte character and writes it to the output stream.
.do end
.desc end
.return begin
The &func function returns the character written or, if a write error
occurs, the error indicator is set and &func returns
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the wide character written or, if a write
error occurs, the error indicator is set and &wfunc returns
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
    c = fgetc( fp );
    while( c != EOF ) {
        putchar( c );
        c = fgetc( fp );
    }
    fclose( fp );
  }
.exmp end
.class ANSI
.system
