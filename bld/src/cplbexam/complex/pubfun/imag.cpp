#include <complex.h>

void main( void ) {

    Complex    a (24, 27);

    cout << "The imaginary component of " << a << " = "
         << a.imag() << endl;
}
