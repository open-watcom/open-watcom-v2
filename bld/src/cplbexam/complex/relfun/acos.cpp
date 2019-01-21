#include <complex.h>

int main( void ) {

    Complex    a (24, 27);

    cout << "The arccosine of " << a << " = "
         << acos( a ) << endl;
}
