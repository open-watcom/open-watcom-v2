.func chdir _chdir _wchdir
.synop begin
#include <sys/types.h>
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
.do end
.el .do begin
#include <direct.h>
.do end
int chdir( const char *path );
.ixfunc2 '&Direct' &funcb
.if &'length(&_func.) ne 0 .do begin
int _chdir( const char *path );
.ixfunc2 '&Direct' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wchdir( const wchar_t *path );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
.if '&machsys' eq 'QNX' .do begin
The
.id &funcb.
function changes the current working directory to the
specified
.arg path
.ct .li .
The
.arg path
can be either relative to the current working directory or it can be
an absolute path name.
.do end
.el .do begin
.ix 'current directory'
The
.id &funcb.
function changes the current directory on the specified
drive to the specified
.arg path
.ct .li .
.ix 'current drive'
If no drive is specified in
.arg path
then the current drive is assumed.
The
.arg path
can be either relative to the current directory on the specified drive
or it can be an absolute path name.
.np
.ix 'current working directory'
.ix 'default drive'
Each drive under DOS, OS/2 or Windows has a current directory.
The current working directory is the current directory of the current
drive.
If you wish to change the current drive, you must use the
.kw _dos_setdrive
function.
.do end
.im ansiconf
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns zero if successful.
Otherwise, &minus.1 is returned,
.kw errno
is set to indicate the error, and the current working directory
remains unchanged.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.if '&machsys' eq 'QNX' .do begin
.term EACCES
Search permission is denied for a component of
.arg path
.ct .li .
.term ENAMETOOLONG
The argument
.arg path
exceeds {PATH_MAX} in length, or a pathname component is longer than
{NAME_MAX}.
.do end
.term ENOENT
The specified
.arg path
does not exist or
.arg path
is an empty string.
.if '&machsys' eq 'QNX' .do begin
.term ENOMEM
Not enough memory to allocate a control structure.
.term ENOTDIR
A component of
.arg path
is not a directory.
.do end
.endterm
.error end
.see begin
.seelist chdir chmod _dos_setdrive getcwd mkdir
.seelist mknod rmdir stat umask
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
.exmp break
void main( int argc, char *argv[] )
{
    if( argc != 2 ) {
        fprintf( stderr, "Use: cd <directory>\n" );
        exit( 1 );
    }
.exmp break
    if( chdir( argv[1] ) == 0 ) {
        printf( "Directory changed to %s\n", argv[1] );
        exit( 0 );
    } else {
        perror( argv[1] );
        exit( 1 );
    }
}
.exmp end
.class POSIX 1003.1
.system
