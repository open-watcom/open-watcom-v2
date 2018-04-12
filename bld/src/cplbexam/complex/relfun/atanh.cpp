#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The inverse hyperbolic tangent of " << a << " = "
         << atanh( a ) << endl;
}
