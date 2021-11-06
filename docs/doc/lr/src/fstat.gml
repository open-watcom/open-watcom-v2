.func fstat _fstat _fstati64
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
.synop end
.desc begin
The
.id &funcb.
functions obtain information about an open file whose file
&handle is
.arg &fd
.period
This information is placed in the structure located at the address
indicated by
.arg buf
.period
.im statdesc
.im ansiconf
.np
The
.reffunc _fstati64
function differ from
.id &funcb.
in the type of structure that they are
asked to fill in.
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
.seelist fstat creat dup dup2 fcntl
.if '&machsys' eq 'QNX' .do begin
.seelist fsys_stat fsys_fstat
.do end
.seelist fstat open _sopen stat pipe
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
.class POSIX 1003.1
.system
