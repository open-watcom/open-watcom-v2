#include "fail.h"
#include <stddef.h>

#ifdef __WATCOM_INT64__
typedef __int64 I64;
#else
typedef long I64;
#endif

template <class T>
    struct S {
	char c;
	T x;
    };

char x1;
S<short> xs[5];
char x2;
S<int> xi[5];
char x3;
S<long> xl[5];
char x4;
S<float> xf[5];
char x5;
S<double> xd[5];
char x6;
S<I64> x64[5];

void test( unsigned x, void *b, void *f, unsigned s ) {
    unsigned a = unsigned(b);
    unsigned d = ((char*)f) - ((char*)b);
    if( d % s ) _fail;
    if( x % s ) _fail;
    if( a % s ) _fail;
}

int main() {
    test( offsetof( S<short>, x ), xs, &xs[0].x, sizeof(short) );
    test( offsetof( S<int>, x ), xi, &xi[0].x, sizeof(int) );
    test( offsetof( S<long>, x ), xl, &xl[0].x, sizeof(long) );
    test( offsetof( S<float>, x ), xf, &xf[0].x, sizeof(float) );
    test( offsetof( S<double>, x ), xd, &xd[0].x, sizeof(double) );
    test( offsetof( S<I64>, x ), x64, &x64[0].x, sizeof(I64) );
    _PASS;
}
