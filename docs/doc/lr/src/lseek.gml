.func lseek _lseek _lseeki64
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
.if '&machsys' ne 'QNX' .do begin
#include <stdio.h>
.do end
#include <&iohdr>
&off_t lseek( int &fd, &off_t offset, int origin );
.ixfunc2 '&OsIo' &funcb
.if &'length(&_func.) ne 0 .do begin
&off_t _lseek( int &fd, &off_t offset, int origin );
.ixfunc2 '&OsIo' &_func
__int64 _lseeki64( int &fd, __int64 offset, int origin );
.ixfunc2 '&OsIo' &func64
.do end
.synop end
.*
.desc begin
The
.id &funcb.
function sets the current file position at the operating
system level.
The file is referenced using the file &handle
.arg &fd
returned by a successful execution of one of the
.if '&machsys' eq 'QNX' .do begin
.kw creat
.ct,
.kw dup
.ct,
.kw dup2
.ct,
.kw fcntl
.ct,
.kw open
or
.kw sopen
.do end
.el .do begin
.kw creat
.ct,
.kw dup
.ct,
.kw dup2
.ct,
.kw open
or
.kw sopen
.do end
functions.
The value of
.arg offset
is used as a relative offset from a file position determined by the
value of the argument
.arg origin
.ct .li .
.np
The new file position is determined in a manner dependent upon the
value of
.arg origin
which may have one of three possible values (defined in the
.if '&machsys' eq 'QNX' .do begin
.hdrfile stdio.h
or
.hdrfile unistd.h
.do end
.el .do begin
.hdrfile stdio.h
.do end
header file):
.begterm 12
.termhd1 Origin
.termhd2 Definition
.term SEEK_SET
The new file position is computed relative to the start of the file.
The value of
.arg offset
must not be negative.
.term SEEK_CUR
The new file position is computed relative to the current file position.
The value of
.arg offset
may be positive, negative or zero.
.term SEEK_END
The new file position is computed relative to the end of the file.
.endterm
.np
An error will occur if the requested file position is before the
start of the file.
.np
The requested file position may be beyond the end of the file.
On POSIX-conforming systems, if data is later written at this point,
subsequent reads of data in the gap will return bytes whose value is
equal to zero until data is actually written in the gap.
On systems such DOS and OS/2 that are not POSIX-conforming, data
that are read in the gap have arbitrary values.
.np
Some versions of MS-DOS allow seeking to a negative offset, but it is
not recommended since it is not supported by other platforms and may
not be supported in future versions of MS-DOS.
.np
The
.id &funcb.
function does not, in itself, extend the size of a file (see
the description of the
.kw chsize
function).
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.np
The &func64 function is identical to
.id &funcb.
except that it accepts a
64-bit value for the
.arg offset
argument.
.np
The
.id &funcb.
function can be used to obtain the current file position
(the
.kw tell
function is implemented in terms of &func).
This value can then be used with the
.id &funcb.
function to reset the file
position to that point in the file:
.millust begin
&off_t file_posn;
int &fd;

/* get current file position */
file_posn = lseek( &fd, 0L, SEEK_CUR );
  /* or */
file_posn = tell( &fd );

/* return to previous file position */
file_posn = lseek( &fd, file_posn, SEEK_SET );
.millust end
.np
If all records in the file are the same size, the position of the n'th
record can be calculated and read, as illustrated in the example
included below.
The function in this example assumes records are numbered starting
with zero and that
.arg rec_size
contains the size of a record in the file
(including the record-separator character).
.if '&machsys' eq 'QNX' .do begin
(including the record-separator character).
.do end
.el .do begin
(including the carriage-return character in text files).
.do end
.desc end
.*
.return begin
If successful, the current file position is returned in a
system-dependent manner.
A value of 0 indicates the start of the file.
.np
If an error occurs in
.id &funcb.
(&minus.1L) is returned.
.np
If an error occurs in &func64, (&minus.1I64) is returned.
.np
.im errnoref
.return end
.*
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The
.arg &fd
argument is not a valid file &handle..
.term EINVAL
The
.arg origin
argument is not a proper value, or the resulting file offset
would be invalid.
.if '&machsys' eq 'QNX' .do begin
.term ESPIPE
The
.arg &fd
argument is associated with a pipe or FIFO.
.do end
.endterm
.error end
.*
.see begin
.im seeioos
.see end
.*
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>

int read_record( int  &fd,
                 long rec_numb,
                 int  rec_size,
                 char *buffer )
{
    if( lseek( &fd, rec_numb * rec_size, SEEK_SET )
         == -1L ) {
        return( -1 );
    }
    return( read( &fd, buffer, rec_size ) );
}
.exmp break
void main( void )
{
    int  &fd;
    int  size_read;
    char buffer[80];
.exmp break
    /* open a file for input */
.if '&machsys' eq 'QNX' .do begin
    &fd = open( "file", O_RDONLY );
.do end
.el .do begin
    &fd = open( "file", O_RDONLY | O_TEXT );
.do end
    if( &fd != -1 ) {
.exmp break
        /* read a piece of the text */
        size_read = 
	    read_record( &fd, 1, 80, buffer );
.exmp break
        /* test for error */
        if( size_read == -1 ) {
            printf( "Error reading file\n" );
        } else {
            printf( "%.80s\n", buffer );
        }
.exmp break
        /* close the file */
        close( &fd );
    }
}
.exmp end
.*
.ansiname &_func
.class POSIX 1003.1
.system
