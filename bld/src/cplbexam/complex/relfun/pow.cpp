#include <complex.h>

void main( void ) {

    Complex    a (23.4, 24.7), b (45.3, 34);
    double     c = 26.3;
    int        d = 5;

    cout << a << " to the power " << b << " = " << pow( a, b ) << endl;
    cout << a << " to the power " << c << " = " << pow( a, c ) << endl;
    cout << c << " to the power " << a << " = " << pow( c, a ) << endl;
    cout << a << " to the power " << d << " = " << pow( a, d ) << endl;
}
