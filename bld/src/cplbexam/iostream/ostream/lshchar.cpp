#include <iostream.h>

void main( void ) {

    int      i = 0;
    char     c[] = "Watcom C/C++ compiler.";

    while( c[i] != 0 ) {
        cout << c[i++];
    }
    cout << endl;
}
