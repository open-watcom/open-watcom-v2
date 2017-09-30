#include "dump.h"

struct S {
    int s;
    S( int i ){ s = i; GOOD;};
};

struct INTER {
    int i;
    INTER( S in ) { i = 4; GOOD };
};

struct T {
    T( INTER ){GOOD};
};

void goo( T ) {};

int main()
{
    S s(2);	
    CHECK_GOOD(5);

    T t( s );	
    CHECK_GOOD(5+10+14)

    T t2( S(2) );	
    CHECK_GOOD(5+10+14+5+10+14);

    T t3( s );
    CHECK_GOOD(5+10+14+5+10+14+10+14);

    T t4( S(2) );	
    CHECK_GOOD(5+10+14+5+10+14+10+14+5+10+14);

    return errors != 0;
}

// MSVC++ v4.1  passes this test both /Za and with extensions
// Watcom 11.0 passes this test both -za and -ze

// this test checks that src->inter->tgt via two ctors will be accepted
// for both direct and copy initialization which is actually
// variable initialization
