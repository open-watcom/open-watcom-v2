#include <complex.h>

void main( void ) {

    Complex    a (24, 27);

    cout << "The inverse hyperbolic cosine of " << a << " = "
         << acosh( a ) << endl;
}
