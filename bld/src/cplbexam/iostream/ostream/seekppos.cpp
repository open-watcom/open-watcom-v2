#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    ch[20];
    int     pos = 7;

    fstream    test ( "temp.txt", ios::in|ios::out );
    test << "Hello, my world." << endl;
    test.seekg( 0 );
    while( (test >> ch).good() ) {
        cout << ch << " " << flush;
    }
    cout << endl;
    test.clear();
    test.seekp( pos );
    test << "Open Watcom C++ users." << endl;
    test.seekg( 0 );
    while( (test >> ch).good() ) {
        cout << ch << " " << flush;
    }
    cout << endl;
}
