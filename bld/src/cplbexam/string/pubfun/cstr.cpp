#include <string.hpp>

void main( void ) {

    String         s ("Watcom C++");
    char const    *pch1, *pch2;

    pch1 = s.operator char const *();   // S1
    pch2 = ( const char * )s;             // S2
    // S1 and S2 are the same
    cout << "The first string is \"" << pch1 << "\"" << endl;
    cout << "The other string is \"" << pch2 << "\"" << endl;
}

