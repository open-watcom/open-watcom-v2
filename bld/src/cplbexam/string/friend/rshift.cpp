#include <strstrea.h>
#include <string.hpp>

void main( void ) {

    istrstream    input ( "Open Watcom C++");
    String        s;

    input >> s;
    cout << "The string is \"" << s << "\"" << endl;
}
