#include <iostream.h>

void main( void ) {

    int      i = 0;
    char     c[] = "Open Watcom C/C++ compiler.";

    while( c[i] != 0 ) {
        cout << c[i++];
    }
    cout << endl;
}
