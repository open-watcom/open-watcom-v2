#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    s[20];

    fstream    test;
    test.open( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( -8, ios::end );
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> s).good() ) {
        cout << s << " " << flush;
    }
}
