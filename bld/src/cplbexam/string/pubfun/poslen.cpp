#include <string.hpp>

void main( void ) {

    String    s ("Open Watcom C++ compiler");
    int       pos = 14;
    int       len = 4;

    cout << "The original string: \"" << s << "\"" << endl;
    cout << "The string starts at offset " << pos << " with the length "
         << len << " of the original string: \"" << s.operator ()( pos, len )
         << "\"" << endl;
}

