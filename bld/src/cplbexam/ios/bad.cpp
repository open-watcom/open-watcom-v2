#include <iostream.h>
#include <fstream.h>

int main( void ) {

    fstream    test ( "temp.txt", ios::in|ios::out );
    if( !test.bad() ) {
        test << "Hello my world!" << endl;
        cout << "\"temp.txt\" is created." << endl;
    }
}
