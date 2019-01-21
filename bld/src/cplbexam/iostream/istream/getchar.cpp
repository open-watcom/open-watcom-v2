#include <iostream.h>

int main( void ) {

    char    ch;

    cout << "Enter a string:" << endl;
    do {
        cin.get( ch );
        cout << ch;
    } while( ch != '\n' );
}
