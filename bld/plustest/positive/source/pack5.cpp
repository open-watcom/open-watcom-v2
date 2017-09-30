#include "fail.h"

struct S {
    int n;
#if __WATCOM_REVISION__ >= 8
    _CD a[];
#endif
};

typedef char A[];

A x = { 'a', 'b', 'c' };
unsigned xs = sizeof( x );
A y = { '1', '2', '3', '4', '5' };
unsigned ys = sizeof( y );

struct C1 {
    int x;
    A q;
};
unsigned s1 = sizeof( C1 );
struct C2 {
    int x;
    A w;
};
unsigned s2 = sizeof( C2 );

int main() {
    S *p;
    S *q;

    if( xs != 3 ) fail(__LINE__);
    if( ys != 5 ) fail(__LINE__);
    if( sizeof( C1 ) != sizeof( int ) ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    if( sizeof( C2 ) != sizeof( int ) ) fail(__LINE__);
#endif
    p = new S;
    q = new S[10];
    if( _CD_count != 0 ) fail(__LINE__);
    delete p;
    delete [] q;
    if( _CD_count != 0 ) fail(__LINE__);
    _PASS;
}
