.func fseek
.synop begin
#include <stdio.h>
int fseek( FILE *fp, long int offset, int where );
.ixfunc2 '&StrIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function changes the read/write position of the file
specified by
.arg fp
.ct .li .
This position defines the character that will be read or written on
the next I/O operation on the file.
The argument
.arg fp
is a file pointer returned by
.kw fopen
or
.kw freopen
.ct .li .
The argument
.arg offset
is the position to seek to relative to one of three positions
specified by the argument
.arg where
.ct .li .
Allowable values for
.arg where
are:
.begterm 8
.termhd1 Value
.termhd2 Meaning
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
The
.id &funcb.
function clears the end-of-file indicator and undoes any
effects of the
.kw ungetc
function on the same file.
.np
The
.kw ftell
function can be used to obtain the current position in the file before
changing it.
The position can be restored by using the value returned by
.kw ftell
in a subsequent call to
.id &funcb.
with the
.arg where
parameter set to
.kw SEEK_SET
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns zero if successful, non-zero otherwise.
.im errnoref
.return end
.see begin
.seelist fseek fgetpos fopen fsetpos ftell
.see end
.exmp begin
.blktext begin
The size of a file can be determined by the following example which
saves and restores the current position of the file.
.blktext end
.blkcode begin
#include <stdio.h>
.exmp break
long int filesize( FILE *fp )
  {
    long int save_pos, size_of_file;
.exmp break
    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
  }
.exmp break
void main()
  {
    FILE *fp;

    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      printf( "File size=%ld\n", filesize( fp ) );
      fclose( fp );
    }
  }
.blkcode end
.exmp end
.class ANSI
.system
