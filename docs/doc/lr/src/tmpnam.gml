.func tmpnam _wtmpnam
.synop begin
#include <stdio.h>
char *tmpnam( char *buffer );
.ixfunc2 '&FileOp' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wtmpnam( wchar_t *buffer );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &func.
function generates a unique string for use as a valid file
name.
.if &'length(&wfunc.) ne 0 .do begin
The
.id &wfunc.
function is identical to
.id &func.
except that it generates a
unique wide-character string for the file name.
.do end
.if '&machsys' eq 'QNX' .do begin
.np
If the
.kw TMPDIR
environment variable is defined, the environment string is used once
to initialize a prefix for the temporary file name.
If the
.kw TMPDIR
environment variable is not defined, the path "/tmp" is used as a
prefix for the temporary file name.
In either case, if the path does not exist then the current directory
(".") will be used.
The filename component has the following format:
.millust begin
UUUPPPP.NNNN.TMP
.millust end
where:
.begnote
.note UUU
are unique filename letters for the process (starts with
"AAA", then "AAB", etc.),
.note PPPP
is a variable-length string incorporating the process-id (pid),
followed by a ".",
.note NNNN
is a variable-length string incorporating the network-id (nid),
followed by a ".", and
.note TMP
is the suffix "TMP".
.endnote
.np
For example, if the process-id is 0x0056 and the network-id is 0x0234
then the first temporary file name produced resembles one of the
following:
.millust begin
{TMPDIR_string}/AAAFG.BCD.TMP
           /tmp/AAAFG.BCD.TMP
              ./AAAFG.BCD.TMP
.millust end
.do end
.el .do begin
An internal static buffer is used to construct the filename.
.do end
Subsequent calls to
.id &func.
reuse the internal buffer.
.np
The function generates unique filenames for up to
.kw TMP_MAX
calls.
.desc end
.return begin
If the argument
.arg buffer
is a NULL pointer,
.id &func.
returns a pointer to an internal buffer
containing the temporary file name.
If the argument
.arg buffer
is not a NULL pointer,
.id &func.
copies the temporary file name from the
internal buffer to the specified buffer and returns a pointer to the
specified buffer.
It is assumed that the specified buffer is an array of at least
.kw L_tmpnam
characters.
.np
If the argument
.arg buffer
is a NULL pointer, you may wish to duplicate the resulting string
since subsequent calls to
.id &func.
reuse the internal buffer.
.ixfunc strdup
.millust begin
char *name1, *name2;

name1 = strdup( tmpnam( NULL ) );
name2 = strdup( tmpnam( NULL ) );
.millust end
.return end
.see begin
.seelist fopen fopen_s freopen freopen_s mkstemp _mktemp _tempnam tmpfile tmpfile_s tmpnam tmpnam_s
.see end
.exmp begin
#include <stdio.h>

void main()
{
    char filename[ L_tmpnam ];
    FILE *fp;
.exmp break
    tmpnam( filename );
    fp = fopen( filename, "w+b" );
    /* . */
    /* . */
    /* . */
    fclose( fp );
    remove( filename );
}
.exmp end
.class ANSI
.system
