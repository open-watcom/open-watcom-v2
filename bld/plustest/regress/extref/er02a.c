#include "fail.h"
#include "er02.h"

D *p = new D;

void test( D * );

void test( B2I *p ) {
    p->f3( 23 );
}

int main() {
    test( p );
    if( p->d != -23 ) fail(__LINE__);
    _PASS;
}
