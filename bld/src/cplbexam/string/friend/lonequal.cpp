#include <string.hpp>

void main( void ) {

    String    s1 ("Open Watcom C++");
    String    s2 ("Open Watcom C++ compiler");
    String    s3 ("Open Watcom C");
    char     *pch1, *pch2;

    pch1 = "Open Watcom C++";
    pch2 = "Open Watcom C";
    cout << "String \"" << s1 << "\" "
         << ( operator >=( s1, s2 ) ? "is longer than or equals to" :
                                    "is shorter than" )
         << " string \"" << s2 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator >=( s1, s3 ) ? "is longer than or equals to" :
                                    "is shorter than" )
         << " string \"" << s3 << "\"" << endl;
    cout << "String \"" << s1 << "\" "
         << ( operator >=( s1, pch1 ) ? "is longer than or equals to" :
                                      "is shorter than" )
         << " string \"" << pch1 << "\"" << endl;
    cout << "String \"" << pch2 << "\" "
         << ( operator >=( pch2, s1 ) ? "is longer than or equals to" :
                                      "is shorter than" )
         << " string \"" << s1 << "\"" << endl;
}

