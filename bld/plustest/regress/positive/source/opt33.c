#include "fail.h"

#ifdef __WATCOM_INT64__
struct S {
    unsigned __int64 x : 31;
    unsigned __int64 y : 3;
    unsigned __int64 z : 1;
    __int64 sx : 31;
    __int64 sy : 3;
    __int64 sz : 1;
    S() {
	memset( this, -1, sizeof( *this ) );
    }
};

__int64 unsigned rx( S *p ) {
    return p->x;
}
__int64 unsigned ry( S *p ) {
    return p->y;
}
__int64 unsigned rz( S *p ) {
    return p->z;
}
__int64 rsx( S *p ) {
    return p->sx;
}
__int64 rsy( S *p ) {
    return p->sy;
}
__int64 rsz( S *p ) {
    return p->sz;
}
int main() {
    S x;
    unsigned __int64 v;
    v = 0x0;
    v <<= 32;
    v |= 0x7fffffffUL;
    if( rx( &x ) != v ) _fail;
    if( ry( &x ) != 7 ) _fail;
    if( rz( &x ) != 1 ) _fail;
    if( rsx( &x ) != -1 ) _fail;
    if( rsy( &x ) != -1 ) _fail;
    if( rsz( &x ) != -1 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
