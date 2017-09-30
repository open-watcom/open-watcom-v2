#include "fail.h"

#pragma inline_depth(0)

struct S {
    int tag;
    int &data;
    S( int &r ) : data(r), tag(-1) {
    }
    operator int &() {
	return data;
    }
};

int r;

void wint( int x, unsigned line )
{
    if( x != r ) fail( line );
}
void rint( int &x, unsigned line )
{
    if( x != r ) fail( line );
}

int main() {
    S j(r);

    r = __LINE__;
    wint( j, __LINE__ );
    r = __LINE__;
    wint( (int)j, __LINE__ );
    r = __LINE__;
    rint( j, __LINE__ );
    r = __LINE__;
    rint( (int&)j, __LINE__ );
    int &q = j;
    r = __LINE__;
    rint( j, __LINE__ );
    _PASS;
}
