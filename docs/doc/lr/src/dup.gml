.func dup _dup
.synop begin
#include <&iohdr>
int dup( int &fd );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _dup( int &fd );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The
.id &func.
function duplicates the file &handle given by the argument
.arg &fd
.ct .li .
The new file &handle refers to the same open file &handle as the
original file &handle, and shares any locks.
The new file &handle is identical to the original in that it references
the same file or device, it has the same open mode (read and/or write)
and it will have file position identical to the original.
Changing the position with one &handle will result in a changed position
in the other.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to
.id &func.
.
Use
id &_func.
for ANSI/ISO naming conventions.
.do end
.if '&machsys' eq 'QNX' .do begin
.np
The call
.millust begin
    dup_&fd = dup( &fd );
.millust end
.pc
is equivalent to:
.millust begin
    dup_&fd = fcntl( &fd, F_DUPFD, 0 );
.millust end
.do end
.desc end
.return begin
If successful, the new file &handle is returned to be used with the
other functions which operate on the file.
Otherwise, &minus.1 is returned and
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The argument
.arg &fd
is not a valid open file &handle..
.term EMFILE
The number of file &handle.s would exceed {OPEN_MAX}.
.endterm
.error end
.see begin
.im seeioos dup
.see end
.cp 8
.exmp begin
#include <fcntl.h>
#include <&iohdr>

void main( void )
{
    int &fd, dup_&fd;
.exmp break
    &fd = open( "file",
.if '&machsys' eq 'QNX' .do begin
                O_WRONLY | O_CREAT | O_TRUNC,
.do end
.el .do begin
                O_WRONLY | O_CREAT | O_TRUNC | O_TEXT,
.do end
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( &fd != -1 ) {
        dup_&fd = dup( &fd );
        if( dup_&fd != -1 ) {
.exmp break
            /* process file */
.exmp break
            close( dup_&fd );
        }
        close( &fd );
    }
}
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
