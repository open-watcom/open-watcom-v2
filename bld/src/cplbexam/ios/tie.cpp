#include <iostream.h>

void main( void ) {

    int    num;
                                     // Equivalent to:
    cin.tie( &cout );                // cout << "Enter a integer:";
    cout << "Enter a integer: ";     // cout.flush();
    cin >> num;                      // cin >> num;

    cout << "The value of current tie: " << cin.tie() << endl;
    cin.tie( 0 );  // unties the stream
    cout << "The value of current tie: " << cin.tie() << endl;
}

