.func setvbuf
.synop begin
#include <stdio.h>
int setvbuf( FILE *fp,
             char *buf,
             int mode,
             size_t size );
.ixfunc2 '&StrIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function can be used to associate a buffer with the file
designated by
.arg fp
.ct .li .
If this function is used, it must be called after the file has been
opened and before it has been read or written.
The argument
.arg mode
determines how the file
.arg fp
will be buffered, as follows:
.begterm 8
.termhd1 Mode
.termhd2 Meaning
.term .mono _IOFBF
causes input/output to be fully buffered.
.term .mono _IOLBF
causes output to be line buffered (the buffer will be flushed when a
new-line character is written, when the buffer is full, or when
input is requested on a line buffered or unbuffered stream).
.term .mono _IONBF
causes input/output to be completely unbuffered.
.endterm
.pp
If the argument
.arg buf
is not
.mono NULL,
the array to which it points will be used instead of
an automatically allocated buffer.
The argument
.arg size
specifies the size of the array.
.desc end
.return begin
The
.id &funcb.
function returns zero on success, or a non-zero value if an
invalid value is given for
.arg mode
or
.arg size
.ct .li .
.return end
.see begin
.seelist setvbuf fopen setbuf
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
{
  char *buf;
  FILE *fp;
.exmp break
  fp = fopen( "file", "r" );
  buf = (char *) malloc( 1024 );
  setvbuf( fp, buf, _IOFBF, 1024 );
}
.exmp end
.class ISO C
.system
