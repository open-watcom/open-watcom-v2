#include <string.hpp>

void main( void ) {

    String    s ("Watcom C++");

    cout << "String \"" << s << "\" is "
         << ( s.valid() ? "valid." : "invalid!" ) << endl;
}
