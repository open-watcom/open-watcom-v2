#include <complex.h>

void main( void ) {

    Complex    a (24, 27);

    cout << "The hyperbolic tangent of " << a << " = "
         << tanh( a ) << endl;
}
