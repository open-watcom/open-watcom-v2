.func putch
#include <conio.h>
int putch( int c );
.ixfunc2 '&KbIo' &func
.synop end
.desc begin
The &func function writes the character specified by the argument
.arg c
to the console.
.desc end
.return begin
The &func function returns the character written.
.return end
.see begin
.im seeiocon putch
.see end
.exmp begin
#include <conio.h>
#include <stdio.h>

void main()
  {
    FILE *fp;
    int c;
.exmp break
    fp = fopen( "file", "r" );
    if ( fp != NULL ) {
      while( (c = fgetc( fp )) != EOF )
        putch( c );
    }
    fclose( fp );
  }
.exmp end
.class WATCOM
.system
