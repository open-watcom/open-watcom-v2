#include "fail.h"

struct S {
    int n;
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
unsigned s1 = sizeof( struct C1 );
struct C2 {
    int x;
    A w;
};
unsigned s2 = sizeof( struct C2 );

int main() {
    struct S *p;
    struct S *q;

    if( xs != 3 ) fail(__LINE__);
    if( ys != 5 ) fail(__LINE__);
    if( sizeof( struct C1 ) != sizeof( int ) ) fail(__LINE__);
    if( sizeof( struct C2 ) != sizeof( int ) ) fail(__LINE__);
    _PASS;
}
