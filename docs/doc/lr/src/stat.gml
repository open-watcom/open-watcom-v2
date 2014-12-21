.func begin stat _stat _stati64 _wstat _wstati64
.func2 lstat POSIX
.func end
.synop begin
#include <sys/stat.h>
int stat( const char *path, struct stat *buf );
.ixfunc2 '&FileOp' &funcb
.if &'length(&_func.) ne 0 .do begin
int _stat( const char *path, struct _stat *buf );
.ixfunc2 '&FileOp' &_func
.do end
int _stati64( const char *path, struct _stati64 *buf );
.ixfunc2 '&FileOp' &func64
.if &'length(&wfunc.) ne 0 .do begin
int _wstat( const wchar_t *path, struct _stat *buf );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
int _wstati64( const wchar_t *path, struct _stati64 *buf );
.ixfunc2 '&FileOp' &wfunc64
.ixfunc2 '&Wide' &wfunc64
int lstat( const char *path, struct stat *buf );
.ixfunc2 '&FileOp' l&funcb
.synop end
.desc begin
The
.id &funcb.
functions obtain information about the file or directory
referenced in
.arg path
.ct .li .
This information is placed in the structure located at the address
indicated by
.arg buf
.ct .li .
.im statdesc
.im ansiconf
.np
The
.kw _stati64
.ct ,
.kw _wstat
.ct , and
.kw _wstati64
functions differ from
.id &funcb.
in the type of structure that they are
asked to fill in.
The
.kw _wstat
and
.kw _wstati64
functions deal with wide character strings.
The differences in the structures are described above.
The
.kw lstat
function is identical to
.id &funcb.
on non-UNIX platforms.
.desc end
.im statrtn
.error begin
.begterm 12
.term EACCES
Search permission is denied for a component of
.arg path
.ct .li .
.if '&machsys' eq 'QNX' .do begin
.term EIO
A physical error occurred on the block device.
.term ENAMETOOLONG
The argument
.arg path
exceeds {PATH_MAX} in length, or a pathname component is longer than
{NAME_MAX}.
.term ENOENT
The named file does not exist or
.arg path
is an empty string.
.term ENOTDIR
A component of
.arg path
is not a directory.
.do end
.endterm
.error end
.see begin
.seelist stat fstat fsys_stat fsys_fstat
.see end
.exmp begin
#include <stdio.h>
#include <sys/stat.h>

void main()
{
    struct stat buf;
.exmp break
    if( stat( "file", &buf ) != -1 ) {
        printf( "File size = %d\n", buf.st_size );
    }
}
.exmp end
.class POSIX
.system
