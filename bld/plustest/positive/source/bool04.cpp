#include "fail.h"

int ctr = 0;

int f() {
    ctr++;
    return ctr;
}

int main() {
    bool b1 = ( bool ) ( f(), true );
    if( ( ctr != 1 ) || ( b1 != true ) ) fail( __LINE__ );

    bool b2 = ( bool ) ( f(), false );
    if( ( ctr != 2 ) || ( b2 != false ) ) fail( __LINE__ );

    _PASS;
}
