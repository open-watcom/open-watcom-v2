#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The arcsine of " << a << " = "
         << asin( a ) << endl;
}
