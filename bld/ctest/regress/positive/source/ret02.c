#include "fail.h"

int dummy = 2;

void side_effect() {
    ++dummy;
}

int foo( int a ) {
    int v = a + 1;
    return ( side_effect(), v );
}

int main() {
    int i = dummy;
    if( i != 2 ) _fail;
    i = foo( i );
    if( i != dummy ) _fail;
    if( i != 3 ) _fail;
    if( dummy != 3 ) _fail;
    i = foo( i );
    if( i != dummy ) _fail;
    if( i != 4 ) _fail;
    if( dummy != 4 ) _fail;
    _PASS;
}
