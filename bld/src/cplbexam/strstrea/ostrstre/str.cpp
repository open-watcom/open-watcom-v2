#include <strstrea.h>

void main( void ) {

    char          s[20];

    ostrstream    output;                  // initialized an ostrstream object
    output << "Open Watcom C++ ";               // write string into the object

    istrstream    input ( output.str() );  // return the pointer of the buffer
    input >> s;
    cout << "The first string in the ostrstream object is \""
         << s << "\"" << endl;
    input >> s;
    cout << "The second string in the ostrstream object is \""
         << s << "\"" << endl;
}
