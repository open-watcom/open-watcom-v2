#include <iostream.h>

void main( void ) {

    char    buf[50];

    istream    input ( 0 );
    input = cin;
    cout << "Enter a string:" << endl;
    input >> buf;
    cout << "The string you have just entered:" << endl;
    cout << buf << endl;
}
