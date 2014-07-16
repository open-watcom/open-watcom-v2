#include "fail.h"

#if __WATCOM_REVISION__ < 8
#define _CONST
#else
#define _CONST const
#endif

unsigned calls;

void operator delete( void *p ) {
    ++calls;
    free( p );
}
void operator delete[]( void *p ) {
    ++calls;
    free( p );
}

template <class T>
    void foo( T _CONST *p, T _CONST *q ) {
	calls = 0;
	delete p;
	delete [] q;
	if( calls != 2 ) fail(__LINE__);
    }

struct C1 {
    int a[10];
};
struct C2 {
    C2() : sig(0) {
    }
    int sig;
};
struct C3 : _CD {
    int __u;
};

int main() {
    foo( new _CONST C1, new C1[10] );
    foo( new _CONST C2, new C2[10] );
    foo( new _CONST C3, new C3[10] );
    _PASS;
}
