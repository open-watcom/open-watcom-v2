#include <stdio.h>
#include <string.h>
#include <malloc.h>
FILE *open_err_file( char * );

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
