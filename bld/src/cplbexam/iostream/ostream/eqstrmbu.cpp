#include <iostream.h>

void main( void ) {

    ostream    test ( 0 );
    test = cout.rdbuf();
    test << "Hello my world!" << endl;
}

