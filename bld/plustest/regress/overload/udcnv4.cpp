#include "dump.h"
struct S {
    operator char ();
    operator int ();
    operator int () const;
    operator double () const;
    int s;
};
S::operator char () { BAD; return (char)__LINE__; };
S::operator int () { GOOD; return __LINE__; };
S::operator int () const { GOOD; return __LINE__; };
S::operator double () const { BAD; return (double)__LINE__; };

S *p;
S const *q;

void bar( int ) GOOD;

void foo( void )
{
    bar( p->operator int() );
    bar( q->operator int() );
}
int main( void ) {
    S a;
    p = &a;
    q = &a;
    foo();
    CHECK_GOOD( 55 );
    return errors != 0;
}
