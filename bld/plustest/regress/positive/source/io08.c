#include "fail.h"
#include <iostream.h>
#include <fstream.h>

char *fileContents = "a b c d e f\ng h i\n";

#define _FNAME "io08.dmp"

void writeFile( char *name ) {
    FILE *fp = fopen( name, "wb" );
    if( fp == NULL ) {
	fail(__LINE__);
	return;
    }
    fprintf( fp, fileContents );
    fclose( fp );
}

void readFile( char *name ) {
    ifstream file( name, ios::in | ios::binary );
    if( !file ) {
	fail(__LINE__);
	return;
    }
    if(( file.unsetf( ios::skipws ) & ios::skipws ) == 0 ) fail(__LINE__);
    char *p = fileContents;
    for(;;) {
	char c;
	file >> c;
	if( file.eof() ) break;
	//printf( "%x\n", c );
	if( c != *p ) fail(__LINE__);
	if( *p == '\0' ) {
	    fail(__LINE__);
	    break;
	}
	++p;
    }
    if( *p != '\0' ) fail(__LINE__);
}

int main() {
    writeFile( _FNAME );
    readFile( _FNAME );
    _PASS;
}

struct del {
    ~del() {
	remove( _FNAME );
    }
} del_file;
