#include <string.hpp>

void main( void ) {

    String    s ("Open Watcom C++ compaler");
    int       pos = 15;
    char      ch  = 'i';

    cout << "The original string: \""  << s << "\"" << endl;
    cout << "Change the character of offset " << pos
         << " of the string to be " << "'" << ch << "'" << endl;
    s.operator ()( pos ) = ch;
    cout << "Modified string: \""  << s << "\"" << endl;
}

