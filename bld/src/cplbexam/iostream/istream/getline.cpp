#include <iostream.h>

void main( void ) {

    char    *bp;
    int      len = 50;

    for( int k = 0; k < 2; k++ ) {
        bp = new char [len];
        cout << "Write a sentence:" << endl;
        cin.getline( bp, len );
        cout << "The sentence you have just written:" << endl;
        cout.write( bp, cin.gcount() );
        cout << endl;
        delete bp;
    }
}
