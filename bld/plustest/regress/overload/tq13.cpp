#include "dump.h"
int xx;
struct A {
    operator int ();
    operator int () const;
    operator int () volatile;
    operator int () const volatile;
};
A::operator int () { GOOD; return __LINE__; };
A::operator int () const { GOOD; return __LINE__; };
A::operator int () volatile { GOOD; return __LINE__; };
A::operator int () const volatile { GOOD; return __LINE__; };
void g( int ) { GOOD; }
void f( A &a, A const &b, A volatile &c, A const volatile &d ) {
    auto int (A::*mpa)() = &A::operator int;
    auto int (A::*mpb)() const = &A::operator int;
    auto int (A::*mpc)() volatile = &A::operator int;
    auto int (A::*mpd)() const volatile = &A::operator int;
    (a.*mpa)();
    (a.*mpb)();
    (a.*mpc)();
    (a.*mpd)();
    g( a );
    g( b );
    g( c );
    g( d );
}
int main( void ) {
    A a;
    f( a, a, a, a );
    CHECK_GOOD( 136 );
    return errors != 0;
}
