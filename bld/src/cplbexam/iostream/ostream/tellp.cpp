#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    ch;

    fstream    test ( "temp.txt", ios::in|ios::out );
    for( int i = 'A'; i <= 'L'; i++ ) {
        test << ( ch = i );
        cout << ch << "           "
             << "Next character will be written in \"temp.txt\" at position: "
             << test.tellp() << endl;
    }
}
