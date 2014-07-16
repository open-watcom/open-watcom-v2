#include "fail.h"

// This illustrates the structural flaw in our initial default-argument
// implementation.  When the default argument is a reference and the
// corresponding expression is a value, storage for the value must be
// allocated in the caller.
//  (1) the initial implementation transformed the return value of the
//      default-argument function from "T const &" to "T const" for
//      non-class values.
//  (2) For class values, the problem is more difficult, since the expression
//      must be analysed before the default-argument function symbol is
//      allocated.  I handled one special case (CTOR on top of the expression)
//      in TYPE.C (head branch only) -- the general case requires a re-think.
//
// It seems we must analyse the expression before we decide on the type of the
// default function.  This would mean delaying insertion of the return value
// to a later point than is now done.
//
// (JWW)

#pragma inline_depth 0


#if __WATCOM_REVISION__ >= 8
struct S : _CD {
    S();
    ~S();
    int ar[3];
};

S::S() {
    for( int i = 0; i < 3; ++i ) {
	ar[i] = 'a';
    }
}

S::~S() {
    for( int i = 0; i < 3; ++i ) {
	ar[i] = -1;
    }
}

S retn() {
    return S();
}

void poo( S const &r = retn() ) {
    if( r.ar[0] != 'a' ) fail(__LINE__);
}
#endif

int main()
{
#if __WATCOM_REVISION__ >= 8
    poo();
#endif
    _PASS;
}
