#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    ch;

    fstream    test ( "temp.txt", ios::in|ios::out );
    test << "Just for fun!" << endl;
    test.seekg( 0 );
    test.unsetf( ios::skipws );
    while( !(test >> ch).eof() ) {
        cout << ch;
    }
}
