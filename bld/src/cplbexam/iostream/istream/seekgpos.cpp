#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    s[20];
    int     pos = 4;

    fstream    test;
    test.open( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( pos );
    cout << "The content of \"temp.txt\" starts at offset "
         << pos << ":" << endl;
    while( (test >> s).good() ) {
        cout << s << " " << flush;
    }
}
