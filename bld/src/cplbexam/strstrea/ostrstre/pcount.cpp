#include <strstrea.h>

void main( void ) {

    char    *pch, s[20];
    int      len = 30, pc1, pc2;

    pch = new char [len];
    ostrstream    output ( pch, len );
    output << "Watcom C++" << ends;        // write string into the object
    pc1 = output.pcount();
    cout << "There are " << pc1
         << " characters have been written in the object." << endl;

    output << " compiler" << endl;
    pc2 = output.pcount();
    cout << "There are " << pc2
         << " characters have been written in the object." << endl;

    istrstream     input ( output.str(), len );
    input >> s;
    cout  << "The first string in the ostrstream object is \""
          << s << "\"" << endl;
    input >> s;
    cout  << "The second string in the ostrstream object is \""
          << s << "\"" << endl;
    input >> s;
    cout  << "The third string in the ostrstream object is \""
          << s << "\"" << endl;
    delete pch;
}
