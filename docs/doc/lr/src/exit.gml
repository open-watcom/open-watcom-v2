.func exit
#include <stdlib.h>
void exit( int status );
.funcend
.desc begin
The &func function causes normal program termination to occur.
.pp
First, all functions registered by the
.kw atexit
function are called in the reverse order of their registration.
Next, all open files are flushed and closed, and all files created by the
.kw tmpfile
function are removed.
Finally, the return
.arg status
is made available to the parent process.
.if '&machsys' ne 'QNX' .do begin
Only the low order byte of
.arg status
is available on DOS systems.
.do end
The
.arg status
value is typically set to 0 to indicate successful termination and
set to some other value to indicate an error.
.desc end
.return begin
The &func function does not return to its caller.
.return end
.see begin
.seelist exit abort atexit _exit onexit
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main( int argc, char *argv[] )
  {
    FILE *fp;
.exmp break
    if( argc <= 1 ) {
      fprintf( stderr, "Missing argument\n" );
      exit( EXIT_FAILURE );
    }
.exmp break
    fp = fopen( argv[1], "r" );
    if( fp == NULL ) {
      fprintf( stderr, "Unable to open '%s'\n", argv[1] );
      exit( EXIT_FAILURE );
    }
    fclose( fp );
    exit( EXIT_SUCCESS );
  }
.exmp end
.class ANSI
.system
