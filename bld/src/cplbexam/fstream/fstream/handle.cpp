#include <fstream.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>

void main( void ) {

    char    s[20];
    int     handle;

    handle = open( "temp.txt", O_RDWR | O_CREAT | O_TEXT, S_IRWXU );
    fstream    test ( handle );
    test << "This is a text file." << endl;
    test.seekg( 0 );
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> s).good() ) {
        cout << s << " " << flush;
    }
}
