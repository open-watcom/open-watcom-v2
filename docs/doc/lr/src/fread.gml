.func fread
#include <stdio.h>
size_t fread( void *buf,
              size_t elsize,
              size_t nelem,
              FILE *fp );
.ixfunc2 '&StrIo' &func
.synop end
.desc begin
The &func function reads
.arg nelem
elements of
.arg elsize
bytes each from the file specified by
.arg fp
into the buffer specified by
.arg buf
.ct .li .
.desc end
.return begin
The &func function returns the number of complete elements
successfully read.
This value may be less than the requested number of elements.
.pp
The
.kw feof
and
.kw ferror
functions can be used to determine whether the end of the file was
encountered or if an input/output error has occurred.
.im errnoref
.return end
.see begin
.seelist fread fopen feof ferror
.see end
.exmp begin
.blktext begin
The following example reads a simple student record containing
binary data.
The student record is described by the
.id struct student_data
declaration.
.blktext end
.blkcode begin
#include <stdio.h>

struct student_data {
    int  student_id;
    unsigned char marks[10];
};
.exmp break
size_t read_data( FILE *fp, struct student_data *p )
  {
    return( fread( p, sizeof(*p), 1, fp ) );
  }
.exmp break
void main()
  {
    FILE *fp;
    struct student_data std;
    int i;
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( read_data( fp, &std ) != 0 ) {
        printf( "id=%d ", std.student_id );
        for( i = 0; i < 10; i++ )
          printf( "%3d ", std.marks[ i ] );
        printf( "\n" );
      }
      fclose( fp );
    }
  }
.blkcode end
.exmp end
.class ANSI
.system
