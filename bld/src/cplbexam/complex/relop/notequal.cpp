#include <complex.h>

void main( void ) {

    Complex    a (23.4, 26.9), b (34.6, 21.3);
    Complex    c (23.4, 26.9), j (0, 26.9) , k (23.4,0);
    double     x = 23.4, y = 26.9;

    cout << a << (operator !=(a, c) ? " does not equal to " : " equals to ")
         << c << endl;
    cout << x << (operator !=(x, k) ? " does not equal to " : " equals to ")
         << k << endl;
    cout << k << (operator !=(k, x) ? " does not equal to " : " equals to ")
         << x << endl;
    cout << a << (operator !=(a, b) ? " does not equal to " : " equals to ")
         << b << endl;
    cout << y << (operator !=(y, j) ? " does not equal to " : " equals to ")
         << j << endl;
    cout << j << (operator !=(j, y) ? " does not equal to " : " equals to ")
         << y << endl;
}
