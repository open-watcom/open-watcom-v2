#include <complex.h>

void main( void ) {

    Complex    a (23.4, 28);

    cout << a << " is the same as "
         << a.operator +() << endl;
}
