#include <stdio.h>
#include <jstring.h>
#include <errno.h>

void main()
  {
    FILE *fp;

    fp = fopen( "file.nam", "r" );
    if( fp == NULL ) {
        printf( "Unable to open file: %s\n",
                 jstrerror( errno ) );
    }
  }
