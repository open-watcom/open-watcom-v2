#include "fail.h"

struct S {
    int i;
    int j;
    int &r;
    S( int x ) : i(x), j(x), r(i) {}
    S( char x ) : i(x), j(x), r(j) {}
};

void foo( int S::* mp, S *p, int v )
{
    if(( p->*mp ) != v ) fail(__LINE__);
}

int main()
{
    S d1( 1 );
    S d2( 'a' );

    foo( &S::r, &d1, 1 );
    foo( &S::r, &d2, 'a' );
    _PASS;
}
