#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    s[20];

    fstream    test;
    test.open( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( 0 );
    while( test.good() ) {
        cout << "The position of next character available for reading: "
             << test.tellg() << endl;
        test >> s;
        cout << s << endl;
    }
}

