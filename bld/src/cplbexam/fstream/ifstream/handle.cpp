#include <fstream.h>
#include <sys\types.h>                     //
#include <sys\stat.h>                      //   C library
#include <fcntl.h>                         //

void main( void ) {

    char    s[20];
    int     handle;

    handle = open( "temp.txt", O_RDONLY|O_TEXT, S_IRUSR );
    ifstream    input ( handle );  // Using C library file handle as argument
    input >> s;
    cout << "The first string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
    input >> s;
    cout << "The second string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
}

