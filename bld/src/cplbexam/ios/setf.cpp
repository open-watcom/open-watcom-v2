#include <iostream.h>

void main( void ) {

    float    num;

    cout << "Enter a decimal number:" << endl;
    cin >> num;
    cout.setf( ios::floatfield );
    cout.setf( ios::scientific, ios::fixed );  // scientific notation for output
    cout << num << "(scientfic notation)" << endl;
    cout.setf( ios::fixed, ios::scientific );  // floating notation for output
    cout << num << "(floating notation)" << endl;
}
