#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    ch;

    fstream     test  ( "temp.txt", ios::in|ios::out );
    cout << "Write a sentence:" << endl;
    cin.get( *test.rdbuf() );
    cout << "Your sentence has been saved in \"temp.txt\"." << endl;
    test.seekg( 0 );
    test.unsetf( ios::skipws );
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> ch).good() ) {
        cout << ch;
    }
}

