#include "fail.h"

struct S;

S *ident( S a[] ) {
    return a;
}

struct S {
};

int main() {
    S x;
    if( ident( &x ) != &x ) fail(__LINE__);
    _PASS;
}
