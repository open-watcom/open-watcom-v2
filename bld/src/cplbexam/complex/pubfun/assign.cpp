#include <complex.h>

void main( void ) {

    Complex    a (24, 27), p, q;
    double     b = 45.6;

    p = a;                      // Copy the values of a into p
    q = b;                      /* Set the real component of q to b and the
                                   imaginary component be zero */
    cout << p << endl;
    cout << q << endl;
}
