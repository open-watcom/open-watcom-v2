#include "fail.h"

struct S1 {
};
struct S2 {
};

void f( S1 * ) {
    fail(__LINE__);
}
void f( int x ) {
    if( x & 1 ) fail(__LINE__);
}
void f( S2 * ) {
    fail(__LINE__);
}
void f( char x ) {
    if(( x & 1 ) == 0 ) fail(__LINE__);
}

typedef void FI( int );
typedef void FC( char );

int i = 2;
char c = char( ' ' | 1 );

FI *a1[] = { f, &f, (f), (&f), (0,f), (0,&f) };
FC *a2[] = { f, &f, (f), (&f), (0,f), (0,&f) };
FI *v1( f );
FI *v2( &f );
FC *v3( f );
FC *v4( &f );
void ei( FI *p ) {
    p( i += 2 );
}
void ec( FC *p ) {
    p( c += 2 );
}
void test() {
    a1[0]( i += 2 );
    a1[1]( i += 2 );
    a1[2]( i += 2 );
    a1[3]( i += 2 );
    a1[4]( i += 2 );
    a1[5]( i += 2 );
    a2[0]( c += 2 );
    a2[1]( c += 2 );
    a2[2]( c += 2 );
    a2[3]( c += 2 );
    a2[4]( c += 2 );
    a2[5]( c += 2 );
    v1( i += 2 );
    v2( i += 2 );
    v3( c += 2 );
    v4( c += 2 );
    ei( f );
    ei( &f );
    ec( f );
    ec( &f );
}

int main() {
    test();
    _PASS;
}
