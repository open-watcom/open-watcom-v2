#include <iostream.h>

void main( void ) {

    char    stk[20], ech;
    int     i = 0, size = 4;            //default putback size

    cout << "Enter a string: " << endl;
    do {
        ech = cin.get();
        cout << ech;
        stk[i++] = ech;         // store the extracted character
    } while( ech != '\n' );
    // put back the characters into the istream object
    for( int k = i; k > (i-size-1); --k ) {
        cin.putback( stk[k] );
    }
    cout << "The last " << size
         << " characters has been put back into the istream object."
         << endl;
    // reprint the string
    cout << "The characters stored in the object: " << endl;
    for( int j = 0; j < size; j++ ) {
        ech = cin.get();
        cout << ech;
    }
}

