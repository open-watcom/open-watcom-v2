#include <iostream.h>

void main( void ) {

    cout.width( 8 );
    cout <<  "Hello"  << endl;
    cout << "The fill character is '" << cout.fill() << "'" << endl;
    cout.fill( '@' );
    cout.width( 8 );
    cout << "Hello" << endl;
    cout << "The fill character is '" << cout.fill() << "'" << endl;
}
