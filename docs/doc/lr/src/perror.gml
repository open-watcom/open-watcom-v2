.func perror _wperror
#include <stdio.h>
void perror( const char *prefix );
.ixfunc2 '&StrIo' &func
.ixfunc2 '&Errs' &func
.if &'length(&wfunc.) ne 0 .do begin
void _wperror( const wchar_t *prefix );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Errs' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function prints, on the file designated by
.kw stderr
.ct,
the error message corresponding to the error number
contained in
.kw errno
.ct .li .
The &func function writes first the string pointed to by
.arg prefix
to stderr.
This is followed by a colon (":"), a space, the string returned by
.mono strerror(errno)
.ct , and a newline character.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument and produces wide-character output.
.do end
.desc end
.return begin
The &func function returns no value.
Because &func uses the
.kw fprintf
function,
.kw errno
can be set when an error is detected during the execution of
that function.
.return end
.see begin
.seelist &function. clearerr feof ferror perror strerror
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
.exmp break
    fp = fopen( "data.fil", "r" );
    if( fp == NULL ) {
        perror( "Unable to open file" );
    }
  }
.exmp end
.class ANSI
.system
