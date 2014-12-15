.func stackavail _stackavail
.synop begin
#include <malloc.h>
size_t stackavail( void );
.ixfunc2 '&Memory' &funcb
.if &'length(&_func.) ne 0 .do begin
size_t _stackavail( void );
.ixfunc2 '&Memory' &_func
.do end
.synop end
.desc begin
The
.id &funcb.
function returns the number of bytes currently available
in the stack.
This value is usually used to determine an appropriate amount to
allocate using alloca.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The
.id &funcb.
function returns the number of bytes currently available
in the stack.
.return end
.see begin
.seelist stackavail alloca calloc Functions malloc Functions
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <&iohdr>

long char_count( FILE *fp )
{
     char    *buffer;
     size_t  bufsiz;
     long    count;

     /* allocate half of stack for temp buffer */
     bufsiz = stackavail() >> 1;
     buffer = (char *) alloca( bufsiz );
     setvbuf( fp, buffer, _IOFBF, bufsiz );
     count = 0L;
     while( fgetc( fp ) != EOF ) ++count;
     fclose( fp );
     return( count );
}
.exmp break
void main( void )
{
    FILE    *fp;

    fp = fopen( "file", "rb" );
    if( fp != NULL ) {
        setmode( fileno( fp ), O_BINARY );
        printf( "File contains %lu characters\n",
                char_count( fp ) );
        fclose( fp );
    }
}
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
