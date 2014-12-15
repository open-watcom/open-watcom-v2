.func fputs fputws
.synop begin
.funcw fputws
#include <stdio.h>
int fputs( const char *buf, FILE *fp );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
int fputws( const wchar_t *buf, FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function writes the character string pointed to by
.arg buf
to the output stream designated by
.arg fp
.ct .li .
The terminating null character is not written.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it converts the
wide character string specified by
.arg buf
to a multibyte character string and writes it to the output stream.
.do end
.desc end
.return begin
The
.id &funcb.
function returns
.kw EOF
if an error occurs; otherwise, it returns a non-negative value
(the number of characters written).
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns
.kw EOF
if a write or encoding error occurs; otherwise, it returns a
non-negative value (the number of characters written).
.do end
.im errnoref
.return end
.see begin
.seelist fopen fputc fputchar fputs putc putchar puts ferror
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    char buffer[80];
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( fgets( buffer, 80, fp ) != NULL )
        fputs( buffer, stdout );
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
