.func begin
.funct   tmpnam_s   TR 24731
.funct_w _wtmpnam_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
errno_t tmpnam_s( char * s, rsize_t maxsize );
.ixfunc2 '&FileOp' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t _wtmpnam_s( wchar_t * s, rsize_t maxsize );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
.arg s
shall not be a null pointer.
.arg maxsize
shall be less than or equal to
.kw RSIZE_MAX
.ct .li .
.arg maxsize
shall be greater than the length of the generated file name string.
.rtconst end
.*
.desc begin
The
.id &funcb.
function generates a string that is a valid file name and that is not the
same as the name of an existing file. The function is potentially capable of generating
.kw TMP_MAX_S
different strings, but any or all of them may already be in use by existing
files and thus not be suitable return values. The lengths of these strings shall be less than
the value of the
.kw L_tmpnam_s
macro.
The
.id &funcb.
function generates a different string each time it is called.
.np
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function is identical to
.id &funcb.
except that it generates a
unique wide-character string for the file name.
.do end
.desc end
.*
.return begin
If no suitable string can be generated, or if there is a runtime-constraint violation, the
.id &funcb.
function writes a null character to
.arg s[0]
(only if
.arg s
is not null and
.arg maxsize
is greater than zero) and returns a non-zero value.
Otherwise, the
.id &funcb.
function writes the string in the array pointed to by
.arg s
and returns zero.
.return end
.*
.see begin
.seelist fopen fopen_s freopen freopen_s mkstemp _mktemp _tempnam tmpfile tmpfile_s tmpnam
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main()
{
    char    filename[ L_tmpnam_s ];
    FILE    *fp;
    errno_t rc;
.exmp break
    rc = tmpnam( filename, sizeof( filename ) );
    if( rc == 0 ) {
        fp = fopen( filename, "w+b" );
        /* . */
        /* . */
        /* . */
        fclose( fp );
        remove( filename );
    }
}
.exmp end
.classt
.system
