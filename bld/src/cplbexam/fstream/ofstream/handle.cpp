#include <fstream.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void main( void ) {

    char    s[20];
    int     handle;

    handle = open( "temp.txt", O_WRONLY | O_CREAT | O_TEXT, S_IWUSR );
    ofstream    output ( handle );
    output << "This is a text file." << endl;

    ifstream    input ( "temp.txt" );
    cout << "The content of \"temp.txt\":" << endl;
    while( (input >> s).good() ) {
        cout << s << " " << flush;
    }
}
