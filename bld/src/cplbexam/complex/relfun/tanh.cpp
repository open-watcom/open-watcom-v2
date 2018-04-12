#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The hyperbolic tangent of " << a << " = "
         << tanh( a ) << endl;
}
