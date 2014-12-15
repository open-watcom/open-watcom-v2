.func rewind
.synop begin
#include <stdio.h>
void rewind( FILE *fp );
.ixfunc2 '&StrIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function sets the file position indicator for the stream
indicated to by
.arg fp
to the beginning of the file.
It is equivalent to
.millust begin
    fseek( fp, 0L, SEEK_SET );
.millust end
.pc
except that the error indicator for the stream is cleared.
.desc end
.return begin
The
.id &funcb.
function returns no value.
.return end
.see begin
.seelist rewind fopen clearerr
.see end
.exmp begin
#include <stdio.h>

static assemble_pass( int passno )
  {
    printf( "Pass %d\n", passno );
  }
.exmp break
void main()
  {
    FILE *fp;

    if( (fp = fopen( "program.asm", "r")) != NULL ) {
        assemble_pass( 1 );
        rewind( fp );
        assemble_pass( 2 );
        fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
