.func fwrite
#include <stdio.h>
size_t fwrite( const void *buf,
               size_t elsize,
               size_t nelem,
               FILE *fp );
.ixfunc2 '&StrIo' &func
.synop end
.desc begin
The &func function writes
.arg nelem
elements of
.arg elsize
bytes each to the file specified by
.arg fp
.ct .li .
.desc end
.return begin
The &func function returns the number of complete elements
successfully written.
This value will be less than the requested number of elements only if
a write error occurs.
.im errnoref
.return end
.see begin
.seelist fwrite ferror fopen
.see end
.exmp begin
#include <stdio.h>

struct student_data {
    int  student_id;
    unsigned char marks[10];
};
.exmp break
void main()
  {
    FILE *fp;
    struct student_data std;
    int i;
.exmp break
    fp = fopen( "file", "w" );
    if( fp != NULL ) {
      std.student_id = 1001;
      for( i = 0; i < 10; i++ )
        std.marks[ i ] = (unsigned char) (85 + i);
.exmp break
      /* write student record with marks */
      i = fwrite( &std, sizeof(std), 1, fp );
      printf( "%d record written\n", i );
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
