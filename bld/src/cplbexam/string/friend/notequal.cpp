#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom C++");
    String    s2 ("Watcom C++ ");
    String    s3 ("Watcom C++");
    char     *pch1, *pch2;

    pch1 = "Watcom C++";
    pch2 = "watcom C++";
    cout << "String \"" << s1 << "\" "
         << ( operator !=( s1, s2 ) ? "does not equal to" : "equals to" )
         << " string \"" << s2 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator !=( s1, s3 ) ? "does not equal to" : "equals to" )
         << " string \"" << s3 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator !=( s1, pch2 ) ? "does not equal to" : "equals to" )
         << " string \"" << pch2 << "\"" << endl;
    cout << "String \"" << pch1 << "\" "
         << ( operator !=( pch1, s1 ) ? "does not equal to" : "equals to" )
         << " string \"" << s1 << "\"" << endl;
}

