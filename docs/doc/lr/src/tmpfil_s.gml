.func tmpfile_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
errno_t tmpfile_s( FILE * restrict * restrict streamptr);
.ixfunc2 '&StrIo' &funcb
.synop end
.*
.rtconst begin
.arg streamptr
shall not be a null pointer.
If there is a runtime-constraint violation,
.id &funcb.
does not attempt to create a file.
.rtconst end
.*
.desc begin
The
.id &funcb.
function creates a temporary binary file that is different from any other
existing file and that will automatically be removed when it is closed or at program
termination. If the program terminates abnormally, whether an open temporary file is
removed is implementation-defined. The file is opened for update with "wb+" mode
with the meaning that mode has in the fopen_s function (including the mode's effect
on exclusive access and file permissions).
If the file was created successfully, then the pointer to FILE pointed to by
.arg streamptr
will be set to the pointer to the object controlling the opened file. Otherwise, the pointer
to FILE pointed to by
.arg streamptr
will be set to a null pointer.

.if '&machsys' ne 'QNX' .do begin
For all systems except NetWare, the temporary file is located in the
path specified by one of the following environment variables, if one
is defined.
Otherwise, the current working directory is used.
.ix 'environment variable' 'tmpfile_s'
They are listed in the order examined:
.kw TMP
.ct ,
.kw TEMP
.ct ,
.kw TMPDIR
.ct , and
.kw TEMPDIR
.ct .li .
.do end
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist fopen fopen_s freopen freopen_s mkstemp _mktemp _tempnam tmpfile tmpnam tmpnam_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main()
{
    errno_t rc;
    FILE    *TempFile;

    rc = tmpfile_s( &TempFile );
    if( rc == 0 ) {
        /* . */
        /* . */
        /* . */
        fclose( TempFile );
    }
}
.exmp end
.class TR 24731
.system
