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
    T( S ) { GOOD };
};

void goo( T ) {};

int main()
{
    S s(2);	
    CHECK_GOOD(5);
    goo( s );			 // calls T( s );
    CHECK_GOOD(5+15)
    goo( S(2) );	 	 // calls T( S( 2 ) );
    CHECK_GOOD(5+15+5+15);
    return errors != 0;
}

// MSVC++ v4.1 passes both /Za and with extensions
// Watcom 11.0  passes -za and -ze

// this test checks that src->tgt via one ctor is prefered over
// src->inter->tgt via two ctors
