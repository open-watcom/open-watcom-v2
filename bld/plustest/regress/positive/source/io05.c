#include "fail.h"
#include <iostream.h>
#include <strstream.h>

int main() {
    ostrstream x;
    x << '@';
    x.width( 4 );
    x << 'a';
    x << '@' << ends;
    if( strcmp( x.str(), "@   a@" ) != 0 ) fail(__LINE__);
    _PASS;
}
