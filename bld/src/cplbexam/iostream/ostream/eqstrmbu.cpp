#include <iostream.h>

int main( void ) {

    ostream    test ( 0 );
    test = cout.rdbuf();
    test << "Hello my world!" << endl;
}

