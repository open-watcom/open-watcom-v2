.func strerror wcserror _ustrerror
#include <string.h>
char *strerror( int errnum );
.ixfunc2 '&String' &func
.ixfunc2 '&Errs' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *wcserror( int errnum );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Errs' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ustrerror( int errnum );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Errs' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function maps the error number contained in
.arg errnum
to an error message.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that the message it
points to is a wide-character string.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that the
message it points to is a Unicode character string.
.do end
.desc end
.return begin
The &func function returns a pointer to the error message.
The array containing the error string should not be modified by the
program.
This array may be overwritten by a subsequent call to the &func
function.
.return end
.see begin
.seelist &function. clearerr feof ferror perror strerror strerror_s strerrorlen_s
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <errno.h>

void main()
{
    FILE *fp;
.exmp break
    fp = fopen( "file.nam", "r" );
    if( fp == NULL ) {
        printf( "Unable to open file: %s\n",
                 strerror( errno ) );
    }
}
.exmp end
.class ANSI
.system
