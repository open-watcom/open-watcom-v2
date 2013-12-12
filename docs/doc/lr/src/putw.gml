.func _putw
.synop begin
#include <stdio.h>
int _putw( int binint, FILE *fp );
.ixfunc2 '&StrIo' &func
.synop end
.desc begin
The &func function writes a binary value of type
.us int
to the current position of the stream
.arg fp
.ct .li .
&func does not affect the alignment of items in the stream, nor does
it assume any special alignment.
.np
&func is provided primarily for compatibility with previous libraries.
Portability problems may occur with &func because the size of an
.us int
and the ordering of bytes within an
.us int
differ across systems.
.desc end
.return begin
The &func function returns the value written or, if a write error
occurs, the error indicator is set and &func returns
.kw EOF
.ct .li .
Since
.kw EOF
is a legitimate value to write to
.arg fp
.ct , use
.kw ferror
to verify that an error has occurred.
.return end
.see begin
.seelist &function. ferror fopen fputc fputchar fputs
.seelist &function. putc putchar puts _putw
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
