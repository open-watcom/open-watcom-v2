#include <string.hpp>

void main( void ) {

    String    s1 ("Open Watcom C++");
    String    s2 ("Open Watcom C++ ");
    String    s3 ("Open Watcom C++");
    char     *pch1, *pch2;

    pch1 = "Open Watcom C++";
    pch2 = "Open watcom C++";
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

