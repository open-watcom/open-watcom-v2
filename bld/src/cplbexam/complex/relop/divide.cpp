#include <complex.h>

void main( void ) {

    Complex    a (23.4, 26.9), b (34.6, 21.3);
    double     c = 14.7;

    cout << a << " / " << b  << " = "
         << operator /(a, b) << endl;
    cout << a << " / " << c  << " = "
         << operator /(a, c) << endl;
    cout << c << " / " << a  << " = "
         << operator /(c, a) << endl;
}
