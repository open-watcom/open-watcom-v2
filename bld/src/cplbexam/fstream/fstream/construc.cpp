#include <fstream.h>

void main( void ) {

    char    s[20];

    fstream    test ( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( 0 );
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> s).good() ) {
        cout << s << " " << flush;
    }
}

