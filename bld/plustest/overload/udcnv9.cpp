#include "dump.h"

struct S {
    S( int ){GOOD;};
    operator double(){GOOD; return 0.0;};
};

struct T {
    T( int ){GOOD};
};

void foo() {
    S s(2);	
    CHECK_GOOD(4);
    T const & t = T( s ); 	// T(s.operator double())
    CHECK_GOOD(4+5+9);


    // T( S(2) ) is two direct initializations equivalent to
    // S s(2); followed by T(s);
    // the case below is just these two direct initializations
    // followed by a copy init from type T to type T
    T const & r = T( S( 2 ) );	
    CHECK_GOOD(4+5+9+4+5+9);
}

int main()
{
    foo();
    return errors != 0;
}
