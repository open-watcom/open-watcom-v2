
#include <stdio.h>

struct student_data {
    int  student_id;
    unsigned char marks[10];
};

size_t read_data( FILE *fp, struct student_data *p )
  {
    return( fread( p, sizeof(*p), 1, fp ) );
  }

void main()
  {
    FILE *fp;
    struct student_data std;
    int i;

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

