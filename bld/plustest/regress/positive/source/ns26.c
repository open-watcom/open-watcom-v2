#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
char q;
void foo( char c ) {
    extern void foo( int );
    extern char q;
    q = c;
    foo( q );
}
void foo( int x ) {
    if( x != q ) _fail;
    q = 'w';
}

int main() {
    foo( 'e' );
    if( q != 'w' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
