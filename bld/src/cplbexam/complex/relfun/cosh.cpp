#include <complex.h>

void main( void ) {

    Complex    a (24, 27);

    cout << "The hyperbolic cosine of " << a << " = "
         << cosh( a ) << endl;
}
