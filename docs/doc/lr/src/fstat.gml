.func fstat _fstat _fstati64 _wfstat _wfstati64
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
int fstat( int &fd, struct stat *buf );
.ixfunc2 '&OsIo' &funcb
.if &'length(&_func.) ne 0 .do begin
int _fstat( int &fd, struct stat *buf );
.ixfunc2 '&OsIo' &_func
.do end
int _fstati64( int handle, struct _stati64 *buf );
.ixfunc2 '&OsIo' &func64
int _wfstat( int handle, struct _stat *buf );
.ixfunc2 '&OsIo' &wfunc
.ixfunc2 '&Wide' &wfunc
int _wfstati64( int handle, struct _stati64 *buf );
.ixfunc2 '&OsIo' &wfunc64
.ixfunc2 '&Wide' &wfunc64
.synop end
.desc begin
The
.id &funcb.
functions obtain information about an open file whose file
&handle is
.arg &fd
.ct .li .
This information is placed in the structure located at the address
indicated by
.arg buf
.ct .li .
.im statdesc
.im ansiconf
.np
The
.kw _fstati64
.ct ,
.kw _wfstat
.ct , and
.kw _wfstati64
functions differ from
.id &funcb.
in the type of structure that they are
asked to fill in.
The
.kw _wfstat
and
.kw _wfstati64
functions deal with wide character strings.
The differences in the structures are described above.
.desc end
.im statrtn
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The
.arg &fd
argument is not a valid file &handle..
.endterm
.error end
.see begin
.seelist fstat creat dup dup2 fcntl fsys_stat fsys_fstat
.seelist fstat open sopen stat pipe
.see end
.exmp begin
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <&iohdr>
.exmp break
void main()
{
    int &fd, rc;
    struct stat buf;
.exmp break
    &fd = open( "file", O_RDONLY );
    if( &fd != -1 ) {
        rc = fstat( &fd, &buf );
        if( rc != -1 )
            printf( "File size = %d\n", buf.st_size );
        close( &fd );
    }
}
.exmp end
.class POSIX
.system
