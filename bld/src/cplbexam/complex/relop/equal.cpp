#include <complex.h>

void main( void ) {

    Complex    a (23.4, 26.9), b (34.6, 21.3);
    Complex    c (23.4, 26.9), j (0, 26.9) , k (23.4,0);
    double     x = 23.4, y = 26.9;

    cout << a << (operator ==(a, c) ? " equals to " : " does not equal to ")
         << c << endl;
    cout << x << (operator ==(x, k) ? " equals to " : " does not equal to ")
         << k << endl;
    cout << k << (operator ==(k, x) ? " equals to " : " does not equal to ")
         << x << endl;
    cout << a << (operator ==(a, b) ? " equals to " : " does not equal to ")
         << b << endl;
    cout << y << (operator ==(y, j) ? " equals to " : " does not equal to ")
         << j << endl;
    cout << j << (operator ==(j, y) ? " equals to " : " does not equal to ")
         << y << endl;
}
