#include "dump.h"
struct S {
    operator int ();
    operator int () const;
    int s;
};
S::operator int () { GOOD; return __LINE__; };
S::operator int () const { GOOD; return __LINE__; };

S *p;
S const *q;

int foo( void )
{
    return(*p);
}

int bar( void )
{
    return(*q);
}
int main( void ) {
    S a;
    p = &a;
    q = &a;
    foo();
    bar();
    CHECK_GOOD( 15 );
    return errors != 0;
}
