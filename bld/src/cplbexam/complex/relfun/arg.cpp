#include <complex.h>

void main( void ) {

    Complex    a (24, 27);

    cout << "The angle of the vector " << a << " in radian "
         << " = " << arg( a ) << endl;
}
