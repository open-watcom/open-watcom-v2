#include <string.hpp>

int main( void ) {

    String    s ("Open Watcom C++");

    cout << "The first character of the string \"" << s
         << "\" is " << "'" << s.operator char() << "'" << endl;
}
