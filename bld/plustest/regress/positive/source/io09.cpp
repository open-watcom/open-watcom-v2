#include "fail.h"
#include <iostream.h>
#include <strstrea.h>
#include <string.hpp>

int main()
{
    ostrstream sout;
    String	reg, s1, s2, s3;
    reg = "01234567890123456789";
    s1 = reg( 0, 10 );
    s2 = reg( 10, 10 );
    
    s1 = "9876543210";
    s2 = "9876543210";
    sout << "s1: " << s1 << " s2: " << s2 << ends;
    char *p = sout.str();
    if( strcmp( p, "s1: 9876543210 s2: 9876543210" ) != 0 ) fail(__LINE__);
    _PASS;
}
