.func eof _eof
.synop begin
#include <&iohdr>
int eof( int &fd );
.ixfunc2 '&OsIo' &funcb
.if &'length(&_func.) ne 0 .do begin
int _eof( int &fd );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The
.id &funcb.
function determines, at the operating system level, if the
end of the file has been reached for the file whose file &handle is
given by
.arg &fd
.ct .li .
Because the current file position is set following an input operation,
the
.id &funcb.
function may be called to detect the end of the file before
an input operation beyond the end of the file is attempted.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.desc end
.return begin
The
.id &funcb.
function returns 1 if the current file position is at the
end of the file, 0 if the current file position is not at the end.
A return value of &minus.1 indicates an error, and in this case
.kw errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.term EBADF
The
.arg &fd
argument is not a valid file &handle..
.endterm
.error end
.see begin
.seelist eof read
.see end
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>

void main( void )
{
    int &fd, len;
    char buffer[100];
.exmp break
    &fd = open( "file", O_RDONLY );
    if( &fd != -1 ) {
        while( ! eof( &fd ) ) {
            len = read( &fd, buffer, sizeof(buffer) - 1 );
            buffer[ len ] = '\0';
            printf( "%s", buffer );
        }
        close( &fd );
    }
}
.exmp end
.class WATCOM
.system
