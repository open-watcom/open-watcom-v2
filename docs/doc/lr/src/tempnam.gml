.func _tempnam _wtempnam
.synop begin
#include <stdio.h>
char *_tempnam( char *dir, char *prefix );
.ixfunc2 '&FileOp' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wtempnam( wchar_t *dir, wchar_t *prefix );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
.id &func.
creates a temporary filename for use in another directory.
This filename is different from that of any existing file.
The
.arg prefix
argument is the prefix to the filename.
.id &func.
uses
.kw malloc
to allocate space for the filename; the program is responsible for
freeing this space when it is no longer needed.
.id &func.
looks for the file with the given name in the following
directories, listed in order of precedence.
.begnote
.notehd1 Directory Used
.notehd2 Conditions
.note Directory specified by TMP
The
.kw TMP
environment variable must be set and the directory specified by
.kw TMP
must exist.
.note dir (function argument)
The
.kw TMP
environment variable must not be set or the directory specified by
.kw TMP
does not exist.
.note _P_tmpdir (_wP_tmpdir) in STDIO.H
The
.arg dir
argument is NULL or
.arg dir
is the name of a nonexistent directory.
The
.kw _wP_tmpdir
string is used by &wfunc..
.note Current working directory
.id &func.
uses the current working directory when
.kw _P_tmpdir
does not exist.
.id &wfunc.
uses the current working directory when
.kw _wP_tmpdir
does not exist.
.endnote
.np
.id &func.
automatically handles multibyte-character string arguments as
appropriate, recognizing multibyte-character sequences according to
the OEM code page obtained from the operating system.
.if &'length(&wfunc.) ne 0 .do begin
.id &wfunc.
is a wide-character version of
.id &func.
the arguments and return
value of
.id &wfunc.
are wide-character strings.
.id &wfunc.
and
.id &func.
behave identically except that
.id &wfunc.
does not handle
multibyte-character strings.
.do end
.np
The function generates unique filenames for up to
.kw TMP_MAX
calls.
.desc end
.return begin
The
.id &func.
function returns a pointer to the name generated, unless it
is impossible to create this name or the name is not unique.
If the name cannot be created or if a file with that name already
exists,
.id &func.
returns NULL.
.return end
.see begin
.seelist fopen freopen mkstemp _mktemp _tempnam tmpfile tmpnam
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

/*
  Environment variable TMP=C:\WINDOWS\TEMP
*/
void main()
  {
    char *filename;

    FILE *fp;

    filename = _tempnam( "D:\\TEMP", "_T" );
    if( filename == NULL )
        printf( "Can't obtain temp file name\n" );
    else {
        printf( "Temp file name is %s\n", filename );
        fp = fopen( filename, "w+b" );
        /* . */
        /* . */
        /* . */
        fclose( fp );
        remove( filename );
        free( filename );
    }
  }
.exmp output
Temp file name is C:\WINDOWS\TEMP\_T1
.exmp end
.class WATCOM
.system
