#include <fstream.h>
#include <sys\types.h>                     //
#include <sys\stat.h>                      // C library
#include <fcntl.h>                         //

void main( void ) {

    char    s[20], *bp;
    int     handle, len = 20;

    bp = new char[len];
    handle = open( "temp.txt", O_RDONLY|O_TEXT, S_IRUSR );
    ifstream    input ( handle, bp, len );
    input >> s;
    cout << "The first string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
    input >> s;
    cout << "The second string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
    delete bp;
}

