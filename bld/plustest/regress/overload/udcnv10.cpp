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

void foo() {
    S s(2);	
    CHECK_GOOD(10);
    T const & t = T( s ); 	// T(s.operator INTER())
    CHECK_GOOD(10+11+15);
    T const & r = T( S( 2 ) );	// T(S(2).operator INTER())
    CHECK_GOOD(10+11+15+10+11+15);
}

int main()
{
    foo();
    return errors != 0;
}
