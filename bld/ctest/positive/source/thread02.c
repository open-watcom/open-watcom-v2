#include "fail.h"

#if defined( __NT__ ) && defined( __386__ )

#define TLS __declspec( thread )

struct A {
    int a;
    int b;
    int c;
};

TLS struct A v;

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
