#include <string.hpp>

void main( void ) {

    String    s ("Open Watcom C++");

    cout << "The string \"" << s << "\" is "
         << ( valid( s ) ? "valid." : "invalid!" ) << endl;
}
