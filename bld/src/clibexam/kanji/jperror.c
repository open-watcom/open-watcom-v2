#include <stdio.h>
#include <jstring.h>

void main()
  {
    FILE *fp;

    fp = fopen( "data.fil", "r" );
    if( fp == NULL ) {
        jperror( "Unable to open file" );
    }
  }
