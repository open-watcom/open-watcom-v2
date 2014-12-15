.func jperror
.synop begin
#include <jstring.h>
void jperror( const char *prefix );
.ixfunc2 '&StrIo' &funcb
.ixfunc2 '&Errs' &funcb
.synop end
.desc begin
The
.id &funcb.
function prints, on the file designated by
.kw stderr
.ct,
the Kanji error message corresponding to the error number
contained in
.kw errno
.ct .li .
The
.id &funcb.
function is implemented as follows:
.millust begin
#include <stdio.h>
void jperror( const char *prefix )
{
    if( prefix != NULL ) {
        if( *prefix != '\0' ) {
            fprintf( stderr, "%s: ", prefix );
        }
    }
    fprintf( stderr, "%s\n", jstrerror( errno ) );
}
.millust end
.desc end
.return begin
The
.id &funcb.
function returns no value.
Because the function uses the
.kw fprintf
function,
.kw errno
can be set when an error is detected during the execution of
that function.
.return end
.see begin
.seelist jperror jstrerror perror strerror
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    FILE *fp;
.exmp break
    fp = fopen( "data.fil", "r" );
    if( fp == NULL ) {
        jperror( "Unable to open file" );
    }
  }
.exmp end
.class WATCOM
.system
