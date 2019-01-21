#include <string.hpp>

int main( void ) {

    String    s ("Open Watcom C++");

    cout << "The string \"" << s << "\" is "
         << ( valid( s ) ? "valid." : "invalid!" ) << endl;
}
