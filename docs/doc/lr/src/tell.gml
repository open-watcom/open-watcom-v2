.func tell _tell _telli64
#include <&iohdr>
&off_t tell( int &fd );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
off_t _tell( int &fd );
.ixfunc2 '&OsIo' &_func
.do end
__int64 _telli64( int &fd );
.ixfunc2 '&OsIo' &_func64
.funcend
.*
.desc begin
The &func function reports the current file position at the operating
system level.
The
.arg &fd
value is the file &handle returned by a successful execution of the
.kw open
function.
.np
The returned value may be used in conjunction with the
.kw lseek
function to reset the current file position.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.np
The &_func64 function is similar to the &func function but returns a
64-bit file position.
This value may be used in conjunction with the
.kw _lseeki64
function to reset the current file position.
.desc end
.*
.return begin
If an error occurs in &func, (&minus.1L) is returned.
.np
If an error occurs in &_func64, (&minus.1I64) is returned.
.np
.im errnoref
.np
Otherwise, the current file position is returned in a system-dependent
manner.
A value of 0 indicates the start of the file.
.return end
.*
.see begin
.im seeioos tell
.see end
.*
.exmp begin
#include <stdio.h>
#include <sys/stat.h>
#include <&iohdr>
#include <fcntl.h>

char buffer[]
        = { "A text record to be written" };
.exmp break
void main( void )
{
    int &fd;
    int size_written;

    /* open a file for output             */
    /* replace existing file if it exists */
    &fd = open( "file",
.if '&machsys' eq 'QNX' .do begin
                O_WRONLY | O_CREAT | O_TRUNC,
.do end
.el .do begin
                O_WRONLY | O_CREAT | O_TRUNC | O_TEXT,
.do end
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.exmp break
    if( &fd != -1 ) {

        /* print file position */
        printf( "%ld\n", tell( &fd ) );

        /* write the text */
        size_written = write( &fd, buffer,
                              sizeof( buffer ) );

        /* print file position */
        printf( "%ld\n", tell( &fd ) );

        /* close the file */
        close( &fd );
    }
}
.exmp output
0
28
.exmp end
.*
.class WATCOM
.*
.system
