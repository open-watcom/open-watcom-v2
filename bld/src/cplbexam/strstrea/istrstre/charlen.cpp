#include <strstrea.h>

void main( void ) {

    char     *pch = "Open Watcom C++", s[20];
    int       len = 8;

    istrstream     input ( pch, len );
    input >>  s;
    cout  << "The first string in the istrstream object is \""
          << s << "\"" << endl;
    input >> s;
    cout  << "The second string in the istrstream object is \""
          << s << "\"" << endl;
}
