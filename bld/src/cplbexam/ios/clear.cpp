#include <iostream.h>
#include <fstream.h>

void main ( void ) {

    char    ch;
    int     pos = 7;

    fstream    test ( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( 0 );
    test.unsetf( ios::skipws );
    while( (test >> ch).good() ) {
        cout << ch << flush;
    }
    cout << endl;
    test.clear();                              // clear the iostate
    test.seekp( pos );
    test << "Watcom C++ users." << endl;
    test.seekg( 0 );
    while( (test >> ch).good() ) {
        cout << ch << flush;
    }
    cout << endl;
}
