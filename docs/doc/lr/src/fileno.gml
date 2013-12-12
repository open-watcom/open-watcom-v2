.func fileno
#include <stdio.h>
int fileno( FILE *stream );
.ixfunc2 '&OsIo' &func
.synop end
.desc begin
The &func function returns the number of the file &handle for the file
designated by
.arg stream
.ct .li .
This number can be used in POSIX input/output calls anywhere the
value returned by
.kw open
can be used.
The following symbolic values in
.hdrfile &iohdr
define the file &handle.s that are associated with the C language
.if '&machsys' eq 'QNX' .do begin
.ix 'stdin'
.ix 'stdout'
.ix 'stderr'
.us stdin,
.us stdout,
and
.us stderr
.do end
.el .do begin
.ix 'stdin'
.ix 'stdout'
.ix 'stderr'
.ix 'stdaux'
.ix 'stdprn'
.us stdin,
.us stdout,
.us stderr,
.us stdaux,
and
.us stdprn
.do end
files when the application is started.
.if '&machsys' ne 'QNX' .do begin
The
.us stdaux
and
.us stdprn
files are not available for Win32.
.do end
.begterm 20 $compact
.termhd1 Value
.termhd2 Meaning
.term STDIN_FILENO
Standard input file number,
.us stdin
(0)
.term STDOUT_FILENO
Standard output file number,
.us stdout
(1)
.term STDERR_FILENO
Standard error file number,
.us stderr
(2)
.if '&machsys' ne 'QNX' .do begin
.term STDAUX_FILENO
Standard auxiliary file number,
.us stdaux
(3)
.term STDPRN_FILENO
Standard printer file number,
.us stdprn
(4)
.do end
.endterm
.desc end
.return begin
The &func function returns the number of the file &handle for the file
designated by
.arg stream
.ct .li .
If an error occurs, a value of -1 is returned and
.kw errno
is set to indicate the error.
.return end
.see begin
.seelist fileno open
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *stream;
.exmp break
    stream = fopen( "file", "r" );
    printf( "File number is %d\n", fileno( stream ) );
    fclose( stream );
  }
.exmp output
File number is 7
.exmp end
.class POSIX 1003.1
.system
