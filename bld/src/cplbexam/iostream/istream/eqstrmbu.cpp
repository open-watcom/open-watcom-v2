#include <iostream.h>

void main( void ) {

    char    buf[50];

    istream    input ( 0 );
    input = cin.rdbuf();
    cout << "Enter a string:" << endl;
    input.width( sizeof buf );
    input >> buf;
    cout << "The string you have just entered:" << endl;
    cout << buf << endl;
}
