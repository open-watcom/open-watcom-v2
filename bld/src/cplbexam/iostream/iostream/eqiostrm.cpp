#include <iostream.h>

void main( void ) {

    char buf[50];

    iostream input ( 0 ), output ( 0 );
    input  = cin;
    output = cout;
    input.width( sizeof buf );
    cout << "Enter a string:" << endl;
    input  >> buf;
    cout << "The string you have just entered:" << endl;
    output << buf << endl;
}
