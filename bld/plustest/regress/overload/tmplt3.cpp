#include "dump.h"

struct S {};

template<class T> T operator +( S, T ) { GOOD; return T(0); }

int main( void ) {
    S s;
    float f = 1.0;
    int i = 2;
    char c = 3;
    f = s+f;
    i = s+i;
    c = s+c;
    CHECK_GOOD( 15 );
    return errors != 0;
}
