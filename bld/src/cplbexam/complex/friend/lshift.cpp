#include <strstrea.h>
#include <complex.h>

void main( void ) {

    istrstream     input( "(1,2) 3,4 (5)" );
    Complex        a, b, c;

    input >> a;
    input >> b;
    input >> c;
    cout <<   "a = " << a
         << ", b = " << b
         << ", c = " << c << endl;
}
