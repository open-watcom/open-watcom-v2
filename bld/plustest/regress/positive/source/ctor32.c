#include "fail.h"

static int first = 0;
static int second = 0;
static int third = 0;

struct Foo {
    int j;

    Foo() : j(0) { first++; }
    Foo(int i) : j(i) { second ++; }
    Foo(Foo const & foo) : j( foo.j) { third ++; }
};

int main( void )
{
    Foo foo;
    int b = 1;

    if( first != 1 ) fail(__LINE__);
    if( second != 0 ) fail(__LINE__);
    if( second != 0 ) fail(__LINE__);

    Foo temp = b ? 1 : foo; 

    if( temp.j != 1 ) fail(__LINE__);

    if( first != 1 ) fail(__LINE__);
    if( second != 1 ) fail(__LINE__);
    if( second != 1 ) fail(__LINE__);
    _PASS;
}
