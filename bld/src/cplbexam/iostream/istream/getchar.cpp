#include <iostream.h>

void main( void ) {

    char    ch;

    cout << "Enter a string:" << endl;
    do {
        cin.get( ch );
        cout << ch;
    } while( ch != '\n' );
}
