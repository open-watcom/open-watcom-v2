#include "fail.h"
#include <stdlib.h>

#if __WATCOM_REVISION >= 8

struct S : _CD {
    static int d;
    static int foo( int x );
};
int S::d;

static S * volatile p;

int side_effect;

int i;
S *a[20];
S *side_effect1( int x ) {
    ++side_effect;
    rand();
    return a[x];
}

S &side_effect2( int x ) {
    ++side_effect;
    rand();
    return *a[x];
}

int S::foo( int x ) {
    return x + d;
}

int main() {
    a[1] = new S;
    p->d = 1;
    if( p->foo( 1 ) != 2 ) fail(__LINE__);
    (i=0,side_effect1(++i))->d = 2;
    if( side_effect != 1 ) fail(__LINE__);
    if( (i=0,side_effect1(++i))->foo( 2 ) != 4 ) fail(__LINE__);
    if( side_effect != 2 ) fail(__LINE__);
    (i=0,side_effect2(++i)).d = 3;
    if( side_effect != 3 ) fail(__LINE__);
    if( (i=0,side_effect2(++i)).foo( 3 ) != 6 ) fail(__LINE__);
    if( side_effect != 4 ) fail(__LINE__);
    delete a[1];
    _PASS;
}
#else
ALWAYS_PASS
#endif
