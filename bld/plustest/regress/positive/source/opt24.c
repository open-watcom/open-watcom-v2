#include "fail.h"

unsigned foo_calls;

void foo()
{
    ++foo_calls;
}

void t( int b, unsigned line )
{
    static int v;

    if( b != v ) fail( line );
    v = !v;
}

int main() {
    t( ( foo(), 1.0 ) == 2.0, __LINE__ );
    t( ( foo(), 2.0 ) == 2.0, __LINE__ );
    t( ( foo(), 1.0 ) == 2.0, __LINE__ );
    t( ( foo(), 2.0 ) == 2.0, __LINE__ );
    if( foo_calls != 4 ) fail(__LINE__);
    _PASS;
}
