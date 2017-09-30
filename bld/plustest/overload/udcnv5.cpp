#include "dump.h"
struct S {
    operator int ();
    operator double ();
    int s;
};
S::operator int () { GOOD; return __LINE__; };
S::operator double () { GOOD; return __LINE__; };

int (S::* p)() = &(S::operator int);
double (S::* q)() = &(S::operator double);

void foo( S *z )
{
    (z->*p)();
    (z->*q)();
}
int main( void ) {
    S a;
    foo( &a );
    CHECK_GOOD( 15 );
    return errors != 0;
}
