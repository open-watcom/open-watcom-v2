#include <iostream.h>

class mystream : public iostream {
    public:
    mystream ( ios &os ) : iostream ( os ) {};
};

void main( void ) {

    char    buf[50];

    mystream    input ( cin ), output ( cout );
    output << "Enter a string:" << endl;
    input.width( sizeof buf );
    input >> buf;
    output << "The string you have just entered:" << endl;
    output << buf << endl;
}

