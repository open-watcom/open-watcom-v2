.func isatty _isatty
.synop begin
#include <&iohdr>
int isatty( int &fd );
.ixfunc2 '&OsIo' &funcb
.if &'length(&_func.) ne 0 .do begin
int _isatty( int &fd );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The
.id &funcb.
function tests if the opened file or device referenced by
the file &handle
.arg &fd
is a character device (for example, a console, printer or port).
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The
.id &funcb.
function returns zero if the device or file is not
a character device; otherwise, a non-zero value is returned.
.im errnoref
.return end
.see begin
.seelist isatty open
.see end
.exmp begin
#include <stdio.h>
#include <&iohdr>

void main( void )
{
    printf( "stdin is a %stty\n",
            ( isatty( fileno( stdin ) ) )
            ? "" : "not " );
}
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
