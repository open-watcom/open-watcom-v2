#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    ch;

    fstream     test ( "temp.txt", ios::in|ios::out );
    test << "Just for fun!" << endl;
    test.unsetf( ios::skipws );       // turn off the bit to skip white spaces
    test.seekg( 0 );
    cout << "The original content of \"temp.txt\":" << endl;
    while( (test >> ch).good() ) {
        cout << ch << flush;
    }
    cout << endl;

    test.clear();
    test.seekg( 0 );
    cout << "The content of \"temp.txt\" without white spaces:" << endl;
    while( (test >> ws >> ch).good() ) {
        cout << ch << flush;
    }
    cout << endl;

    // The other way to skip white spaces
    test.clear();
    test.seekg( 0 );
    cout << "The content of \"temp.txt\" without white spaces:" << endl;
    while( ( ( test.operator>>( ws ) ) >> ch).good() ) {
        cout << ch << flush;
    }
    cout << endl;
}
