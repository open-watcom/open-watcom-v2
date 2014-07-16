#include "fail.h"

#ifdef __WATCOM_MUTABLE__

int usei( int x ) {
    return x;
}
int main() {
    struct S { int i; int mutable j; };
    static const S x = { 1, 2 };
    const S *px = &x;
    if( px->j != 2 ) fail(__LINE__);
    px->j = usei( 3 );
    if( px->j != usei(3) ) fail(__LINE__);
    _PASS;
}

#else
ALWAYS_PASS
#endif
