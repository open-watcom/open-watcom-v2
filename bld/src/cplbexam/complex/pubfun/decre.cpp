#include <complex.h>

void main( void ) {

    Complex    a (23.4, 28), b (25.7, 56.1);
    double     c = 45.6;
    int        d = 10;

    cout << a << " - " << b << " = " << flush;
    cout << ( a -= b ) << endl;
    cout << b << " - " << c << " = " << flush;
    cout << ( b -= c ) << endl;
    // Now the value of a has been changed
    cout << a << " - " << d << " = " << flush;
    cout << ( a -= d ) << endl;
}
