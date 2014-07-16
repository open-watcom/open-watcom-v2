#include "fail.h"

struct S {
};
#ifdef __WATCOM_NAMESPACE__
namespace N {
};
#else
int N;
#endif
template <class c> struct T {};

struct H {
    unsigned id : 1;
    unsigned S : 1;
    unsigned N : 1;
    unsigned T : 1;
};

void foo( H *p, H &r ) {
    p->id = 1;
    p->S = 1;
    p->N = 1;
    p->T = 1;
    r.id = 1;
    r.S = 1;
    r.N = 1;
    r.T = 1;
}
void bar( H *p, H &r ) {
    if( p->id != 1 ) _fail;
    if( p->S != 1 ) _fail;
    if( p->N != 1 ) _fail;
    if( p->T != 1 ) _fail;
    if( r.id != 1 ) _fail;
    if( r.S != 1 ) _fail;
    if( r.N != 1 ) _fail;
    if( r.T != 1 ) _fail;
}

H x;
H y;

int main() {
    foo( &x, y );
    bar( &x, y );
    _PASS;
}
