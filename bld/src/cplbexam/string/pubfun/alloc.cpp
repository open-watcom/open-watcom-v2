#include <string.hpp>

void main( void ) {

    String    s;

    s = " ";
    cout << "The current allocation mutiple size: " << s.alloc_mult_size() << endl;
    s.alloc_mult_size( 20 );
        cout << "The current allocation mutiple size: "
             << s.alloc_mult_size() << endl;
}

