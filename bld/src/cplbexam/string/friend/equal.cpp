#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom C++");
    String    s2 ("Watcom C++ ");
    String    s3 ("Watcom C++");
    char     *pch1, *pch2;

    pch1 = "Watcom C++";
    pch2 = "watcom C++";
    cout << "String \"" << s1 << "\" "
         << ( operator ==( s1, s2 ) ? "equals to" : "does not equal to" )
         << " string \"" << s2 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator ==( s1, s3 ) ? "equals to" : "does not equal to" )
         << " string \"" << s3 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator ==( s1, pch2 ) ? "equals to" : "does not equal to" )
         << " string \"" << pch2 << "\"" << endl;
    cout << "String \"" << pch1 << "\" "
         << ( operator ==( pch1, s1 ) ? "equals to" : "does not equal to" )
         << " string \"" << s1 << "\"" << endl;
}

