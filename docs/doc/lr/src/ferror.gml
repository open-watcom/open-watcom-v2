.func ferror
#include <stdio.h>
int ferror( FILE *fp );
.ixfunc2 '&StrIo' &func
.ixfunc2 '&Errs' &func
.funcend
.desc begin
The &func function tests the error indicator for the stream
pointed to by
.arg fp
.ct .li .
.desc end
.return begin
The &func function returns non-zero if the error indicator is set for
.arg fp
.ct .li .
.return end
.see begin
.seelist &function. clearerr feof ferror perror strerror
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
      c = fgetc( fp );
      if( ferror( fp ) ) {
        printf( "Error reading file\n" );
      }
    }
    fclose( fp );
  }
.exmp end
.class ANSI
.system
