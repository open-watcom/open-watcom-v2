.func clearerr
.synop begin
#include <stdio.h>
void clearerr( FILE *fp );
.ixfunc2 '&StrIo' &func
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The
.id &func.
function clears the end-of-file and error indicators
for the stream pointed to by
.arg fp
.ct .li .
These indicators are cleared only when the file is opened or by an
explicit call to the
.id &func.
or
.kw rewind
functions.
.desc end
.return begin
The
.id &func.
function returns no value.
.return end
.see begin
.seelist clearerr feof ferror perror strerror
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    int c;
.exmp break
    c = 'J';
    fp = fopen( "file", "w" );
    if( fp != NULL ) {
      fputc( c, fp );
      if( ferror( fp ) ) {  /* if error        */
        clearerr( fp );   /* clear the error */
        fputc( c, fp );   /* and retry it    */
      }
    }
  }
.exmp end
.class ANSI
.system
