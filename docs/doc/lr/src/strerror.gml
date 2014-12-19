.func strerror wcserror
.synop begin
#include <string.h>
char *strerror( int errnum );
.ixfunc2 '&String' &funcb
.ixfunc2 '&Errs' &funcb
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *wcserror( int errnum );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Errs' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function maps the error number contained in
.arg errnum
to an error message.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that the message it
points to is a wide-character string.
.do end
.desc end
.return begin
The
.id &funcb.
function returns a pointer to the error message.
The array containing the error string should not be modified by the
program.
This array may be overwritten by a subsequent call to the &funcb
function.
.return end
.see begin
.seelist clearerr feof ferror perror strerror strerror_s strerrorlen_s
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
.class ISO C
.system
