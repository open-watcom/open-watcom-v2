#include <iostream.h>
#include <fstream.h>

void main( void ) {

    fstream    test ( "temp.txt", ios::in|ios::out );
    if( !test.bad() ) {
        test << "Hello my world!" << endl;
        cout << "\"temp.txt\" is created." << endl;
    }
}
