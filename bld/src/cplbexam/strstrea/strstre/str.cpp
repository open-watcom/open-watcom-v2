#include <strstrea.h>

void main( void ) {

    char    s[20];

    strstream    test;
    test << "Open Watcom C++" << endl;         // write the string into the buffer

    istrstream    input ( test.str() );
    input >> s;             // read the first string from the strstream object
    cout  << "The first string in the strstream object is \""
          << s << "\"" << endl;
    input >> s;             // skip the white space and read the other string
    cout  << "The second string in the strstream object is \""
          << s << "\"" << endl;
}
