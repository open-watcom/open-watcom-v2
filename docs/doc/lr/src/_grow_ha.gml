.func _grow_handles
.synop begin
#include <stdio.h>
int _grow_handles( int new_count );
.ixfunc2 '&OsIo' &func
.synop end
.desc begin
The &func function increases the number of POSIX level files that are
allowed to be open at one time.
The parameter
.arg new_count
is the new requested number of files that are allowed to be opened.
The return value is the number that is allowed to be opened after the
call.
This may be less than, equal to, or greater than the number requested.
If the number is less than, an error has occurred and the errno
variable should be consulted for the reason.
If the number returned is greater than or equal to the number
requested, the call was successful.
.np
Note that even if &func returns successfully, you still might not be
able to open the requested number of files due to some system
limit (e.g. FILES= in the CONFIG.SYS file under DOS) or because some
file handles are already in use (stdin, stdout, stderr, etc.).
.np
.ix 'limits' 'file open'
.ix 'file open limits'
The number of file handles that the run-time system can open by
default is described by
.kw _NFILES
in
.hdrfile stdio.h
but this can be changed by the application developer.
To change the number of file handles available during execution,
follow the steps outlined below.
.autonote
.note
Let
.id n
represent the number of files to be opened concurrently.
Ensure that the
.ix 'stdin'
.us stdin,
.ix 'stdout'
.us stdout,
and
.ix 'stderr'
.us stderr
files are included in the count.
.if '&machsys' ne 'QNX' .do begin
Also include
.ix 'stdaux'
.us stdaux
and
.ix 'stdprn'
.us stdprn
files in the count for some versions of DOS.
The
.us stdaux
and
.us stdprn
files are not available for Win32.
.do end
.if '&machsys' ne 'QNX' .do begin
.note
.ix 'FILES='
For DOS-based systems, change the
.filename CONFIG.SYS
file to include "FILES=n" where "n" is the number of file handles
required by the application plus an additional 5 handles for the
standard files.
The number of standard files that are opened by DOS varies from 3 to 5
depending on the version of DOS that you are using.
.np
If you are running a network such as Novell's NetWare, this will also
affect the number of available file handles.
In this case, you may have to increase the number specified in the
"FILES=n" statement.
.do end
.note
Add a call to &func in your application similar to that shown in the
example below.
.endnote
.desc end
.return begin
The &func function returns the maximum number of file handles which
the run-time system can accommodate.
This number can exceed an operating system
.if '&machsys' eq 'QNX' .do begin
limit.
.do end
.el .do begin
limit such as that imposed by the "FILES=" statement under DOS.
.do end
This limit will be the determining factor in how many files can be
open concurrently.
.return end
.error begin
.error end
.see begin
.seelist &function. _dos_open fdopen fileno fopen freopen _fsopen
.seelist &function. _grow_handles _hdopen open _open_osfhandle _popen
.seelist &function. sopen tmpfile
.see end
.exmp begin
#include <stdio.h>

FILE *fp[ 50 ];
.exmp break
void main()
  {
    int hndl_count;
    int i;
.exmp break
    hndl_count = _NFILES;
    if( hndl_count < 50 ) {
        hndl_count = _grow_handles( 50 );
    }
    for( i = 0; i < hndl_count; i++ ) {
      fp[ i ] = tmpfile();
      if( fp[ i ] == NULL ) break;
      printf( "File %d successfully opened\n", i );
    }
    printf( "%d files were successfully opened\n", i );
  }
.exmp end
.class WATCOM
.system
