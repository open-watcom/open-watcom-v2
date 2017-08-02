#include "dump.h"
struct S {
    virtual operator int ();
    virtual operator double ();
    int s;
};
S::operator int () { BAD; return __LINE__; };
S::operator double () { BAD; return (double)__LINE__; };

struct T : S {
    virtual operator int ();
    virtual operator double ();
    int s;
};
T::operator int () { GOOD; return __LINE__; };
T::operator double () { GOOD; return (double)__LINE__; };

double foo( T *p )
{
    int x;
    double y;

    x = *p;
    y = *p;
    return x + y;
}
int main( void ) {
    T a;
    foo( &a );
    CHECK_GOOD( 31 );
    return errors != 0;
}
