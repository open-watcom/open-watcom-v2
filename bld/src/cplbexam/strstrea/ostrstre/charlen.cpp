#include <strstrea.h>

void main( void ) {

    char     *pch,  s[20];
    int       len = 11;

    pch = new char [len];
    ostrstream     output ( pch, len );    // initialized an ostrstream object
    output << "Open Watcom C++" << endl;        // write string into the object

    istrstream     input ( output.str() ); // return the pointer of the buffer
    input >> s;
    cout << "The first string in the ostrstream object is \""
         << s << "\"" << endl;
    input >> s;
    cout << "The second string in the ostrstream object is \""
         << s << "\"" << endl;
    delete pch;
}
