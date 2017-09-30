#include "fail.h"

#ifdef __WATCOM_RTTI__
struct V {
    int v;
    virtual void foo( void ) {
    }
};
struct D : private virtual V {
    int d;
};
struct E : D, virtual V {
    int e;
};

V *foo( D *p ) {
    return dynamic_cast< V * >( p );
}
#endif

int main() {
#ifdef __WATCOM_RTTI__
    E e;
    V *p = foo( &e );
    if( p == NULL ) _fail;
    if( p != &e ) _fail;
#endif
    _PASS;
}
