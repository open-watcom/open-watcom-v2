#include <iostream.h>

void main( void ) {

    char    ch, nch;

    cout << "Enter a string:" << endl;
    while( ( ch = cin.get() ) != '\n' ) {
        if( ch != '/n' ) {
            cout << "Extracted character: " << ch << endl;
        }
        if( ( nch = cin.peek() ) != '\n' ) {
            cout << "Next character will be extracted: " << nch << endl;
        }
    }
}
