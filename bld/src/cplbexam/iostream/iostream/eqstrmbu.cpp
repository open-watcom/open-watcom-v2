#include <iostream.h>

void main( void ) {

    char    buf[50];

    iostream     input ( 0 ), output ( 0 );
    input  = cin.rdbuf();
    output = cout.rdbuf();
    output << "Enter a string:" << endl;
    input.width( sizeof buf );
    input  >> buf;
    output << "The string you have just entered:" << endl;
    output << buf << endl;
}
