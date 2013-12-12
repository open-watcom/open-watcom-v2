.func fputchar _fputchar _fputwchar
.synop begin
.funcw _fputwchar
#include <stdio.h>
int fputchar( int c );
.ixfunc2 '&StrIo' &func
.if &'length(&_func.) ne 0 .do begin
int _fputchar( int c );
.ixfunc2 '&StrIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
wint_t _fputwchar( wint_t c );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function writes the character specified by the argument
.arg c
to the output stream
.kw stdout
.ct .li .
This function is identical to the
.kw putchar
function.
.np
The function is equivalent to:
.millust begin
    fputc( c, stdout );
.millust end
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
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
    if( fp != NULL ) {
      c = fgetc( fp );
      while( c != EOF ) {
        _fputchar( c );
        c = fgetc( fp );
      }
      fclose( fp );
    }
  }
.exmp end
.class WATCOM
.system
