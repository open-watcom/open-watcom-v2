#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char ch;

    cin >> ch;
    if( cin.sync() == EOF ) {
        cout << "The get area is not clear yet!" << endl;
    } else {
        cout << "Sync succeed!" << endl;
    }
}
