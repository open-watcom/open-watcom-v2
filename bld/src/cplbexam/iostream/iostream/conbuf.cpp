#include <iostream.h>

class mystream : public iostream {
    public:
    mystream ( streambuf *str ) : iostream ( str ) {};
};

void main( void ) {

    char    buf[50];

    mystream    input ( cin.rdbuf() ), output ( cout.rdbuf() );
    output << "Enter a string:" << endl;
    input.get( buf, ( sizeof buf ) );
    output << "The string you have just entered:" << endl;
    output << buf << endl;
}

