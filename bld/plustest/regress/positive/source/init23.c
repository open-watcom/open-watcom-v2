#include "fail.h"

struct B {
    int a,b,c;
};

B Ivan( int i ) {
    B v = { 0, 1, 2 };
    v.b += i;
    return v;
}
void foo() {
    char b[100];

    memset( b, -1, sizeof(b) );
}

int main() {
    foo();
    B x = Ivan( 8 );
    if( x.a != 0 ) _fail;
    if( x.b != 9 ) _fail;
    if( x.c != 2 ) _fail;
    _PASS;
}
