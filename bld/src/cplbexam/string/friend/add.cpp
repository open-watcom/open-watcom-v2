#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom "), s2 ("C++");
    char     *pch1, *pch2;

    pch1 = "Watcom ";
    pch2 = "C++";
    cout << "String \"" << s1 << "\" + string \""
         << s2 << "\" = string \"" << operator +( s1, s2 )
         << "\"" << endl;
    cout << "String \"" << pch1 << "\" + string \""
         << s2 << "\" = string \"" << operator +( pch1, s2 )
         << "\"" << endl;
    cout << "String \"" << s1 << "\" + string \""
         << pch2 << "\" = string \"" << operator +( s1, pch2 )
         << "\"" << endl;
}
