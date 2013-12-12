.func alloca
.synop begin
#include <malloc.h>
void *alloca( size_t size );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
The
.id &func.
function allocates space for an object of
.arg size
bytes from the stack.
The allocated space is automatically discarded when the current
function exits.
The
.id &func.
function should not be used in an expression that is an
argument to a function.
.desc end
.return begin
The
.id &func.
function returns a pointer
to the start of the allocated memory.
The return value is
.mono NULL
if there is insufficient stack space available.
.return end
.see begin
.seelist alloca calloc malloc stackavail
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <malloc.h>
FILE *open_err_file( char * );
.exmp break
void main()
  {
    FILE *fp;

    fp = open_err_file( "alloca" );
    if( fp == NULL ) {
      printf( "Unable to open error file\n" );
    } else {
      fclose( fp );
    }
  }
.exmp break
FILE *open_err_file( char *name )
  {
     char *buffer;
     /* allocate temp buffer for file name */
     buffer = (char *) alloca( strlen(name) + 5 );
     if( buffer ) {
       sprintf( buffer, "%s.err", name );
       return( fopen( buffer, "w" ) );
     }
     return( (FILE *) NULL );
  }
.exmp end
.class WATCOM
.system
