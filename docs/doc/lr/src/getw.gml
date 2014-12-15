.func _getw
.synop begin
#include <stdio.h>
int _getw( int binint, FILE *fp );
.ixfunc2 '&StrIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function reads a binary value of type
.us int
from the current position of the stream
.arg fp
and increments the associated file pointer to point to the next unread
character in the input stream.
.id &funcb.
does not assume any special alignment of items in the stream.
.np
.id &funcb.
is provided primarily for compatibility with previous libraries.
Portability problems may occur with
.id &funcb.
because the size of an
.us int
and the ordering of bytes within an
.us int
differ across systems.
.desc end
.return begin
The
.id &funcb.
function returns the integer value read or, if a read error
or end-of-file occurs, the error indicator is set and
.id &funcb.
returns
.kw EOF
.ct .li .
Since
.kw EOF
is a legitimate value to read from
.arg fp
.ct , use
.kw ferror
to verify that an error has occurred.
.return end
.see begin
.seelist ferror fgetc fgetchar fgets fopen
.seelist getc getchar gets _putw ungetc
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    int c;
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( (c = _getw( fp )) != EOF )
          _putw( c, stdout );
      fclose( fp );
    }
  }
.exmp end
.class WATCOM
.system
