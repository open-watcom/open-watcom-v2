#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom C++ compiler");
    String    s2 ("Watcom C   compiler");
    char     *pch = "Watcom C++ compaler";

    cout << "\"" << s1 << "\" and \""
         << s2 << "\" differs at offset "
         << s1.match( s2 ) << endl;
    cout << "\"" << s1 << "\" and \""
         << pch << "\" differs at offset "
         << s1.match( pch ) << endl;
}
