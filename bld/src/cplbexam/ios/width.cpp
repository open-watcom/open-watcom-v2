#include <iostream.h>

void main( void ) {

    char    *bp;
    int      len = 20;

    bp = new char [len];
    /* read until a whitespace character is found or the maximum size has
       been read */
    cin.width( len );
    cout << "Enter a string:" << endl;
    cin >> bp;
    cout << "The string you have just entered:" << endl;
    cout << bp << endl;
    delete bp;
}
