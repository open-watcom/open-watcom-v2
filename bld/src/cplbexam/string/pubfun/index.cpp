#include <string.hpp>

void main( void ) {

    String    s_A ("Watcom C++ compiler");
    String    s_B ("C++");
    char     *pch = "com";

    cout << "String A: \"" << s_A << "\"\n"

         << "String B \""  << s_B << "\" is at the offset "
         << s_A.index( s_B ) << " of the string A.\n"

         << "The string C \"" << pch << "\" is at the offset "
         << s_A.index( pch )  << " and " << s_A.index( pch, 7 )
         << " of the string A." << endl;
}
