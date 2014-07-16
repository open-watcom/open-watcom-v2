#include "fail.h"

#ifdef __WATCOM_BOOL__

int ok;

void f( void *a ) {
    int *p = (int *)a;
    *p = __LINE__;
}

void f( bool ) {
    fail(__LINE__);
}

int main() {
    f( &ok );
    if( ok <= 0 || ok >= __LINE__ ) fail(__LINE__);
    _PASS;
}

#else
ALWAYS_PASS
#endif
