#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace B {
    struct g { int g; };
    void g(char x)       // OK: hides struct g
    {
	if( x != 'a' ) _fail;
    }
    void h( struct g *p ) {
	p->g = -34;
    }
}
int main() {
    using B::g;
    g('a');             // calls B::g(char)
    struct g g1;        // g1 has class type B::g
    B::h( &g1 );
    if( g1.g != -34 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
