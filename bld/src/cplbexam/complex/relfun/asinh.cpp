#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The inverse hyperbolic sine of " << a << " = "
         << asinh( a ) << endl;
}
