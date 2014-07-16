#include "fail.h"
#include <unistd.h>
#include <fcntl.h>
#include <fstream.h>
#include <stdio.h>

#define TEMP_FILE_1     "io01.obj"
#define TEMP_FILE_2     "io02.obj"

#define FILE_SIZE       (DEFAULT_BUF_SIZE * 3)

int main()
{
    ifstream infile;
    ofstream create_file;
    ofstream outfile;
    int i;
    int f1;
    int f2;
    long p1;
    long p2;
    
    create_file.open( TEMP_FILE_1 );
    for( i = 0; i < FILE_SIZE; ++i ) {
        create_file << char( 'a' + ( i % 26 ) );
    }
    create_file.close();
    infile.open( TEMP_FILE_1 );
    outfile.open( TEMP_FILE_2 );
    outfile << infile.rdbuf();  // should copy entire file
    infile.close();
    outfile.close();
    f1 = open( TEMP_FILE_1, O_RDONLY | O_BINARY );
    if( f1 == -1 ) fail( __LINE__ );
    f2 = open( TEMP_FILE_2, O_RDONLY | O_BINARY );
    if( f2 == -1 ) fail( __LINE__ );
    p1 = lseek( f1, 0, SEEK_END );
    if( p1 == -1 ) fail( __LINE__ );
    p2 = lseek( f2, 0, SEEK_END );
    if( p2 == -1 ) fail( __LINE__ );
    if( p1 != p2 ) fail( __LINE__ );
    if( p1 != FILE_SIZE ) fail( __LINE__ );
    if( p2 != FILE_SIZE ) fail( __LINE__ );
    if( close( f1 ) == -1 ) fail( __LINE__ );
    if( close( f2 ) == -1 ) fail( __LINE__ );
    //if( unlink( TEMP_FILE_1 ) == -1 ) fail( __LINE__ );
    if( unlink( TEMP_FILE_2 ) == -1 ) fail( __LINE__ );
    _PASS;
}
