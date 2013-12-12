.func fclose
#include <stdio.h>
int fclose( FILE *fp );
.ixfunc2 '&StrIo' &func
.synop end
.desc begin
The &func function closes the file
.arg fp
.ct .li .
If there was any unwritten buffered data for the file, it is written out
before the file is closed.
Any unread buffered data is discarded.
If the associated buffer was automatically allocated, it is deallocated.
.desc end
.return begin
The &func function returns zero if the file was successfully
closed, or non-zero if any errors were detected.
.im errnoref
.return end
.see begin
.seelist &function. fclose fcloseall fdopen fopen freopen _fsopen
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
.exmp break
    fp = fopen( "stdio.h", "r" );
    if( fp != NULL ) {
        fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
