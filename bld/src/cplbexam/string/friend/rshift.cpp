#include <strstrea.h>
#include <string.hpp>

int main( void ) {

    istrstream    input ( "Open Watcom C++");
    String        s;

    input >> s;
    cout << "The string is \"" << s << "\"" << endl;
}
