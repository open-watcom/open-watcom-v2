// Test case: overload\udcnv11.cpp
// Status: work complete
#include "dump.h"
struct B {
    B( B & ) {};
};

struct D : public B {
    D( D const & );
};
extern B b1;

struct X {
    operator D();
    operator B(){GOOD; return b1; };
};

X x;

void main()
{
    B b = x;
    CHECK_GOOD(13);
}

// read 13.3.3.1.4
// JWW believes B( B &) should be rejected becuase it needs a temporary from
// operator B() to be bound to a non-const ref (the param of of B(B&) )

// actually, this is copy init not derived, so we're not allowed to
// use a UDC on param of B( B & )

// UDCF to consider:
// operator D(), and then to-base
// operator B() : better ranking, this should win

// Microsoft: cannot implicitly convert X -> B & that is not const
// EDG: no errors
// Borland: no errors
