#include "fail.h"

struct SSSS {
    int d;
    int a[10];
};
struct SSS {
    int c;
    SSSS x;
};
struct SS {
    int b;
    SSS x;
};
struct S {
    int a;
    SS x;
};

void foo() {
    S p;
    p.x.x.x.d = 1;
}

S v = { 'a', { 'b', { 'c', { 'd', 1, 2, 3, 4 }, }, }, };

int main() {
    if( v.a != 'a' ) fail(__LINE__);
    if( v.x.b != 'b' ) fail(__LINE__);
    if( v.x.x.c != 'c' ) fail(__LINE__);
    if( v.x.x.x.d != 'd' ) fail(__LINE__);
    if( v.x.x.x.a[0] != 1 ) fail(__LINE__);
    if( v.x.x.x.a[1] != 2 ) fail(__LINE__);
    if( v.x.x.x.a[2] != 3 ) fail(__LINE__);
    if( v.x.x.x.a[3] != 4 ) fail(__LINE__);
    if( v.x.x.x.a[4] != 0 ) fail(__LINE__);
    if( v.x.x.x.a[9] != 0 ) fail(__LINE__);
    _PASS;
}
