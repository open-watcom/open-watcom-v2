.func read _read
.synop begin
#include <&iohdr>
int read( int &fd, void *buffer, unsigned len );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _read( int &fd, void *buffer, unsigned len );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
The &func function reads data at the operating system level.
The number of bytes transmitted is given by
.arg len
and the data is transmitted starting at the address specified by
.arg buffer
.ct .li .
.np
The
.arg &fd
value is returned by the
.kw open
function.
The access mode must have included either
.kw O_RDONLY
or
.kw O_RDWR
when the
.kw open
function was invoked.
The data is read starting at the
current file position for the file in question.
This file position can be determined with the
.kw tell
function and can be set with the
.kw lseek
function.
.np
When
.kw O_BINARY
is included in the access mode, the data is transmitted unchanged.
When
.kw O_TEXT
is included in the access mode, the data is transmitted with the extra
carriage return character removed before each linefeed character
encountered in the original data.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The &func function returns the number of bytes of data transmitted
from the file to the
buffer (this does not include any carriage-return characters that were
removed during the transmission).
Normally, this is the number given by the
.arg len
argument.
When the end of the file is encountered before the read completes, the
return value will be less than the number of bytes requested.
.np
A value of &minus.1 is returned when an input/output error is
detected.
.im errnoref
.return end
.see begin
.seelist read close creat fread open write
.see end
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>
.exmp break
void main( void )
{
    int  &fd;
    int  size_read;
    char buffer[80];
.exmp break
    /* open a file for input              */
.if '&machsys' eq 'QNX' .do begin
    &fd = open( "file", O_RDONLY );
.do end
.el .do begin
    &fd = open( "file", O_RDONLY | O_TEXT );
.do end
    if( &fd != -1 ) {
.exmp break
        /* read the text                      */
        size_read = read( &fd, buffer,
                          sizeof( buffer ) );
.exmp break
        /* test for error                     */
        if( size_read == -1 ) {
            printf( "Error reading file\n" );
        }
.exmp break
        /* close the file                     */
        close( &fd );
    }
}
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
