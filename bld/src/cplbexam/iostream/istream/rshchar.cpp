#include <iostream.h>

void main( void ) {

    char    ch;

    cin.unsetf( ios::skipws );   // turn off the function to skip whitespaces
    cout << "Enter a string: " << endl;
    do {
        cin >> ch;
        cout << ch;
    } while( ch != '\n' );
}
