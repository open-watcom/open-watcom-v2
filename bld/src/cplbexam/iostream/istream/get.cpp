#include <iostream.h>

void main( void ) {

    char    ch;

    cout << "Enter a string:" << endl;
    while( ( ch = cin.get() ) != '\n' ) {
        cout << ch;
    }
    cout << endl;
}
