#include <string.hpp>

void main( void ) {

    String    s ("Open Watcom C++ compiler");

    cout << "String \"" << s << "\" in lower-case: "
         << "\"" <<  s.lower() << "\"" << endl;
}

