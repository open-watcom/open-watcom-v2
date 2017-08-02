#include "dump.h"

struct INTER {
    int i;
    INTER(int in_i) { i = in_i; };
};

struct S {
    int s;
    S( int ){GOOD;};
    operator INTER() {GOOD; return INTER(s); };
};

struct T {
    T( INTER ){GOOD};
};

void foo( T ) {}
int main()
{
    S s(2);	
    CHECK_GOOD(10);

    T t1( s ); 		// T(s.operator INTER())
    CHECK_GOOD(10+11+15);

    T t2( S( 2 ) );	// T(S(2).operator INTER())
    CHECK_GOOD(10+11+15+10+11+15);

    T t3 = s; 		// T(s.operator INTER())
    CHECK_GOOD(10+11+15+10+11+15+11+15);

    T t4 = S( 2 );	// T(S(2).operator INTER())
    CHECK_GOOD(10+11+15+10+11+15+11+15+10+11+15);

    return errors != 0;
}


// MSVC++ v4.1 passes

// this test checks that src->inter->tgt via a ctor and a UDCF will be
// accepted for both direct initialization and copy initialization
// which is actually variable initialization
