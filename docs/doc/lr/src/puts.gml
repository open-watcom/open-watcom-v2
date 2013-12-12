.func puts _putws
.synop begin
.funcw _putws
#include <stdio.h>
int puts( const char *buf );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
int _putws( const wchar_t *bufs );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function writes the character string pointed to by
.arg buf
to the output stream designated by
.kw stdout
.ct,
and appends a new-line character to the output.
The terminating null character is not written.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &func.
except that it converts the
wide character string specified by
.arg buf
to a multibyte character string and writes it to the output stream.
.do end
.desc end
.return begin
The
.id &func.
function returns
.kw EOF
if an error occurs; otherwise, it returns a non-negative value
(the number of characters written including the new-line character).
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function returns
.kw EOF
if a write or encoding error occurs; otherwise, it returns a
non-negative value (the number of characters written including the
new-line character).
.do end
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
    char buffer[80];
.exmp break
    fp = freopen( "file", "r", stdin );
    while( gets( buffer ) != NULL ) {
        puts( buffer );
    }
    fclose( fp );
  }
.exmp end
.class ANSI
.system
