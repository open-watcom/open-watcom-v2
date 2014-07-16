#include "fail.h"
#include "er01.h"

void throw_X( void ) {
    throw X(23);
}

int main() {
    int s = c();
    if( s != 23 ) fail(__LINE__);
    _PASS;
}
