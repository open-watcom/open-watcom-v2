#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom C"), s2 ("/C++");
    char     *pch = " compiler";

    s1 += s2;
    cout << s1 << endl;
    s1 += pch;
    cout << s1 << endl;
}
