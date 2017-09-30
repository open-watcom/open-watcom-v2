#include "fail.h"

// edit with Windows NOTEPAD!

int main() {
    int c;
    int pc;
    FILE *fp;

    fp = fopen( __FILE__, "r" );
    pc = '\n';
    for(;;) {
	c = fgetc( fp );
	if( c == EOF ) break;
	pc = c;
    }
    fclose( fp );
    // this source file tests terminating C++ comments
    // at the end of a file without a newline
    if( pc == '\n' ) fail(__LINE__);
    _PASS;
}
// this source file tests terminating C++ comments
// at the end of a file without a newline