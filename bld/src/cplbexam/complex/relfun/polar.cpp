#include <complex.h>

void main( void ) {

    Complex    a (24.98, 30);

    cout << "The polar coordinates of " << a << " = "
         << polar( a.real(), a.imag() ) << endl;
}
