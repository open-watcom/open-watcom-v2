#include <fstream.h>
#include <sys\types.h>                     //
#include <sys\stat.h>                      // C library
#include <fcntl.h>                         //

void main( void ) {

    char    s[20], *bp;
    int     handle, len = 20;

    bp = new char[len];
    handle = open( "temp.txt", O_RDWR | O_CREAT | O_TEXT, S_IRWXU );
    fstream    test ( handle, bp, len );
    test << "Just for fun!" << endl;
    test.seekg( 0 );
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> s).good() ) {
        cout << s << " " << flush;
    }
    delete bp;
}

