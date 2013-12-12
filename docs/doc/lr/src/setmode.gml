.func setmode _setmode
.synop begin
#include <&iohdr>
#include <fcntl.h>
int setmode( int &fd, int mode );
.ixfunc2 '&OsIo' &func
.if &'length(&_func.) ne 0 .do begin
int _setmode( int &fd, int mode );
.ixfunc2 '&OsIo' &_func
.do end
.synop end
.desc begin
.if '&machsys' eq 'QNX' .do begin
The &func is provided for compatibility with other systems.
&func performs no useful action under &machsys..
.do end
.el .do begin
The &func function sets, at the operating system level, the
translation mode to be the value of
.arg mode
for the file whose file &handle is given by
.arg &fd
.ct .li .
The mode, defined in the
.hdrfile fcntl.h
header file, can be one of:
.begterm 10
.termhd1 Mode
.termhd2 Meaning
.term .mono O_TEXT
On input, a carriage-return character that immediately precedes a
linefeed character is removed from the data that
is read.
On output, a carriage-return character is inserted before each
linefeed character.
.term .mono O_BINARY
Data is read or written unchanged.
.endterm
.do end
.desc end
.return begin
.if '&machsys' eq 'QNX' .do begin
&func always returns
.kw O_BINARY
under &machsys..
This manifest is defined in the
.hdrfile fcntl.h
header file.
.do end
.el .do begin
If successful, the &func function returns the previous mode that was
set for the file; otherwise, &minus.1 is returned.
.im errnoref
.do end
.return end
.see begin
.im seeioos setmode
.see end
.exmp begin
#include <stdio.h>
#include <fcntl.h>
#include <&iohdr>

void main( void )
{
    FILE *fp;
    long count;
.exmp break
    fp = fopen( "file", "rb" );
    if( fp != NULL ) {
        setmode( fileno( fp ), O_BINARY );
        count = 0L;
        while( fgetc( fp ) != EOF ) ++count;
        printf( "File contains %lu characters\n",
                count );
        fclose( fp );
    }
}
.exmp end
.class WATCOM
.system
