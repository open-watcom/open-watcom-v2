#include <string.hpp>

void main( void ) {

    String    s1 ("Watcom C++"), s2, s3;
    char     *pch = "Watcom C++ compiler";

    s2 = s1;
    s3 = pch;
    cout << s2 << endl;
    cout << s3 << endl;
}
