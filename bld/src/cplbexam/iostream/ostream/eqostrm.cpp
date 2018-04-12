#include <iostream.h>

int main( void ) {

    ostream    test ( 0 );
    test = cout;
    test << "Hello my world!" << endl;
}

