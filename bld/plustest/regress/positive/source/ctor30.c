#include "fail.h"

struct S {
    int s;
    S( int );
    S( S const & );
    ~S();
};

int c;

S::S( int s ) : s(s) {
    ++c;
}
S::S( S const &s ) : s(s.s) {
    if( this == &s ) fail(__LINE__);
    ++c;
}
S::~S(){
    --c;
    if( c < 0 ) fail(__LINE__);
    s = -1;
}

void check( S const &x, unsigned line ) {
    if( c != 2 ) fail( line );
}

// 5.2.3 S(x) == (S)x
// 5.2.8 S -> S is an implicit cast
// 4 - equivalent to: S t(x); with result equal to 't'
int main() {
    S x(__LINE__);
    check( S(x), __LINE__ );
    if( c != 1 ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    check( (S)x, __LINE__ );
    if( c != 1 ) fail(__LINE__);
#endif
    _PASS;
}
