.func setbuf
#include <stdio.h>
void setbuf( FILE *fp, char *buffer );
.ixfunc2 '&StrIo' &func
.funcend
.desc begin
The &func function can be used to associate a buffer with the file
designated by
.arg fp
.ct .li .
If this function is used, it must be called after the file has been
opened and before it has been read or written.
If the argument
.arg buffer
is
.mono NULL,
then all input/output for the file
.arg fp
will be completely unbuffered.
If the argument
.arg buffer
is not
.mono NULL,
then it must point to an array that is at least
.kw BUFSIZ
characters in length, and all input/output will be fully buffered.
.desc end
.return begin
The &func function returns no value.
.return end
.see begin
.seelist setbuf fopen setvbuf
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char *buffer;
    FILE *fp;
.exmp break
    fp = fopen( "file", "r" );
    buffer = (char *) malloc( BUFSIZ );
    setbuf( fp, buffer );
    /* . */
    /* . */
    /* . */
    fclose( fp );
  }
.exmp end
.class ANSI
.system
