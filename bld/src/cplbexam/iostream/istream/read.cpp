#include <iostream.h>

void main( void ) {

    int len = 20;
    char *bp;

    bp = new char [len];
    cout << "Enter a string (Press CTRL-Z): " << endl;
    cin.read( bp, len );
    cout << "The string you have just entered: " << endl;
    cout.write( bp, cin.gcount() );
    delete bp;
}

