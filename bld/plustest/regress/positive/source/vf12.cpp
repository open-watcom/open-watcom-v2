#include "fail.h"

struct S1 {
    int v;
    virtual operator int() {
	return v + 1;
    }
    S1( int v ) : v(v) {
    }
};

struct S2 : S1 {
    virtual operator long()
    {
	return v - 1;
    }
    S2( int v ) : S1(v) {
    }
};

struct S3 : S2 {
    operator int() {
	return v + 2;
    }
    virtual operator long()
    {
	return v - 2;
    }
    S3( int v ) : S2(v) {
    }
};

int main()
{
    S2 *p = new S3(3);
    S1 *q = p;

    if( int(*q) != 5 ) fail(__LINE__);
    if( long(*q) != 5 ) fail(__LINE__);
    if( int(*p) != 5 ) fail(__LINE__);
    if( long(*p) != 1 ) fail(__LINE__);
    _PASS;
}
