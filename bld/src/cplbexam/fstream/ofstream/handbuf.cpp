#include <fstream.h>
#include <sys/types.h>                     //
#include <sys/stat.h>                      // C library
#include <fcntl.h>                         //

void main( void ) {

    char s[20], *bp;
    int  handle, len = 20;

    bp = new char[len];
    handle = open( "temp.txt", O_WRONLY | O_CREAT | O_TEXT, S_IWUSR );
    ofstream    output ( handle, bp, len );
    output << "Just for fun!" << endl;

    ifstream    input ( "temp.txt" );
    cout << "The content of \"temp.txt\":" << endl;
    while( (input >> s).good() ) {
        cout << s << " " << flush;
    }
    delete bp;
}

