#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
struct V {
    void foo( char x );
};
struct D : virtual V {
    using V::foo;
    void foo( int );
};
struct E : virtual V {
};

V *pV;
D *pD;

struct F : virtual D, virtual E {
    F() {
	pV = this;
	pD = this;
    }
};

void V::foo( char x ) {
    if( x != '0' ) _fail;
    if( this != pV ) _fail;
}
void D::foo( int x ) {
    if( x != -2 ) _fail;
    if( this != pD ) _fail;
}

void foo( D *p ) {
    p->foo( -2 );
    p->foo( '0' );
}

int main() {
    F x;
    foo( &x );
    _PASS;
}
#else
ALWAYS_PASS
#endif
