#include <iostream.h>
#include <fstream.h>

void main( void ) {

    fstream    test ( "temp.txt", ios::in|ios::out );
    if( !test.bad() ) {
        test << "Hello my world!" << endl;
    }
    if( test.good() ) {
        cout << "The iostate is clear!" << endl;
    }
}
