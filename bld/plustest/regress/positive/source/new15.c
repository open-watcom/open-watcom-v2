#include "fail.h"

unsigned calls;

void operator delete( void *p ) {
    ++calls;
    free( p );
}
void operator delete[]( void *p ) {
    ++calls;
    free( p );
}

struct U;
#if __WATCOM_REVISION__ < 8
struct U {
    int a[10];
};
#endif

void foo( U *p, U *q ) {
    delete p;
    delete [] q;
}

#if __WATCOM_REVISION__ >= 8
struct U {
    int a[10];
};
#endif

int main() {
    calls = 0;
    foo( new U, new U[10] );
    if( calls != 2 ) fail(__LINE__);
    _PASS;
}
