.func perror _wperror
.synop begin
#include <stdio.h>
void perror( const char *prefix );
.ixfunc2 '&StrIo' &funcb
.ixfunc2 '&Errs' &funcb
.if &'length(&wfunc.) ne 0 .do begin
void _wperror( const wchar_t *prefix );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Errs' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function prints, on the file designated by
.kw stderr
.ct,
the error message corresponding to the error number
contained in
.kw errno
.ct .li .
The
.id &funcb.
function writes first the string pointed to by
.arg prefix
to stderr.
This is followed by a colon (":"), a space, the string returned by
.mono strerror(errno)
.ct , and a newline character.
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns no value.
Because
.id &funcb.
uses the
.kw fprintf
function,
.kw errno
can be set when an error is detected during the execution of
that function.
.return end
.see begin
.seelist clearerr feof ferror perror strerror
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
.class ISO C
.system
