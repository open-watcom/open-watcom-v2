#include "dump.h"

struct A {
    A() GOOD;
};

struct B {
    operator A () { GOOD; return A(); }
    operator int () { BAD; return 0; }
};

void f( A ) GOOD;
void f( long ) BAD;

int main( void ) {
    B b;
    f( b );
    CHECK_GOOD( 4+8+12 );
    return errors != 0;
}

// confirm that a UDCF from class to class which is exact match will
// be preferred over one from class to build-in, when the resulting
// build-in requires an integral conversion (conversion rank standard
// conversion).
