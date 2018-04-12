#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The arctangent of " << a << " = "
         << atan( a ) << endl;
}
