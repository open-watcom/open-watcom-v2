#include <iostream.h>

void main( void ) {

    char *bp;
    int  len = 20;

    bp = new char [len];
    cout << "Enter a string:" << endl;
    cin.get( bp, len );
    cout.write( bp, cin.gcount() );
    delete bp;
}
