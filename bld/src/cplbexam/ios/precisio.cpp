#include <iostream.h>

void main( void ) {

    float    num;
    int      prec = 6;

    cout << "Enter a decimal number:" << endl;
    cin >> num;
    for( int k = 0 ; k < 3 ; k++ ) {
        cout << "The current format precision is "
             <<  cout.precision() << endl;
        cout.setf( ios::scientific );        // scientific notation for output
        cout << num << "(scientfic notation)" << endl;
        cout.unsetf( ios::scientific );
        cout.setf( ios::fixed );             // floating notation for output
        cout << num << "(floating notation)" << endl;
        cout.unsetf( ios::fixed );
        prec = prec + 2;
        cout.precision( prec );
    }
}
