#include "fail.h"

#if defined( __NT__ ) && defined( __386__ ) && ( __WATCOM_REVISION__ >= 8 )

#define TLS __declspec( thread )

struct A {
    int a;
    int b;
    int c;
    int foo(int x) { return( x + 1 ); }
};

TLS A v;

void set() {
    v.a = 1;
    v.b = 2;
    v.c = 3;
}

int main() {
    set();
    if( v.a != 1 ) _fail;
    if( v.b != 2 ) _fail;
    if( v.c != 3 ) _fail;
    _PASS;
}

#else
ALWAYS_PASS
#endif
