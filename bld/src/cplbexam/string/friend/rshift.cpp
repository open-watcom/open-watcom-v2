#include <strstrea.h>
#include <string.hpp>

void main( void ) {

    istrstream    input ( "Watcom C++");
    String        s;

    input >> s;
    cout << "The string is \"" << s << "\"" << endl;
}
