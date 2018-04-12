#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The imaginary component of " << a << " = "
         << imag( a ) << endl;
}
