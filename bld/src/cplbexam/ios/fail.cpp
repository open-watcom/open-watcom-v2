#include <iostream.h>
#include <fstream.h>

void main( void ) {

    fstream    test ( "temp.txt", ios::in|ios::out );  // S1

    /* S2: fstream    test ("temp.txt", ios::in);
       Compare the results by using S2 instead of S1  */

    if( !test.bad() ) {
        test << "Hello my world!" << endl;
        cout << "\"temp.txt\" is created." << endl;
    }
    if( test.fail() ) {
        cout << "The previous operation on the stream fails!" << endl;
    }
}
