#include "fail.h"

int count;

struct S {
    int s;
    S(int);
    S(S const &);
    ~S();
};

S::S( int x ) : s(x) {
    ++count;
}

S::S( S const &r ) : s(r.s) {
    ++count;
}

S::~S() {
    --count;
    if( count < 0 ) fail(__LINE__);
}

void foo()
{
    // only the static foo's S should stay constructed
    static S foo( S(1) );
}

int main()
{
    foo();
    if( count != 1 ) fail(__LINE__);
    foo();
    if( count != 1 ) fail(__LINE__);
    foo();
    if( count != 1 ) fail(__LINE__);
    foo();
    if( count != 1 ) fail(__LINE__);
    foo();
    if( count != 1 ) fail(__LINE__);
    foo();
    if( count != 1 ) fail(__LINE__);
    _PASS;
}
