#include "fail.h"

#if __WATCOM_REVISION__>7
#pragma pack(8);
#else
#pragma pack(1);
#endif

struct X1 {
    short	a[3];
    char	*b;
};

X1 v1[] = {
    1,2,3, "4",
    5,6,7, "8",
    9,10,11, "12",
    13,14,15, "16",
};

void check_X1( X1 *p, int b, unsigned line ) {
    if( p->a[0] != b++ ) fail(line);
    if( p->a[1] != b++ ) fail(line);
    if( p->a[2] != b++ ) fail(line);
    if( atol( p->b ) != b ) fail(line);
}

struct X2 {
    char	a[2];
    long	b;
    char	c[3];
};

void check_X2( X2 *p ) {
    int b = 1;
    if( p->a[0] != b++ ) fail(__LINE__);
    if( p->a[1] != b++ ) fail(__LINE__);
    if( p->b != b++ ) fail(__LINE__);
    if( p->c[0] != b ) fail(__LINE__);
    if( p->c[1] != 0 ) fail(__LINE__);
    if( p->c[2] != 0 ) fail(__LINE__);
}

void test() {
    X2 v = { 1, 2, 3, 4 };
    check_X2( &v );
    check_X2( &v );
}

union X3 {
    char a;
    long b;
};
X3 v2[2] = { 1, -1 };

void check_X3( void ) {
    if( v2[0].a != 1 ) fail(__LINE__);
    v2[0].a = 0;
    if( v2[0].b != 0 ) fail(__LINE__);
    if( v2[1].a != char(-1) ) fail(__LINE__);
    v2[1].a = 0;
    if( v2[1].b != 0 ) fail(__LINE__);
}

int main() {
    test();
    check_X1( &v1[0], 1, __LINE__ );
    check_X1( &v1[1], 5, __LINE__ );
    check_X1( &v1[2], 9, __LINE__ );
    check_X1( &v1[3], 13, __LINE__ );
    check_X3();
    _PASS;
}
