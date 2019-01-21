#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The square of the magnitude of " << a << " = "
         << norm( a ) << endl;
}
