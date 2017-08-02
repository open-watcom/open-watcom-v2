#include "fail.h"

// we don't handle bitfields in static anonymous unions
static union {
    unsigned char x:2;
    int q;
};

struct S {
    union {
	unsigned char x:2;
	int q;
    };
};

int test( S *p )
{
    union {
	unsigned char y:2;
	int q;
    };
    q = 0;
    return ( x == 3 ) && ( p->x == 2 ) && ( y == 0 );
}

int main() {
    S x;
    q = -1;
    x.q = -1;
    x.x = 2;
    if( test( &x ) != 1 ) _fail;
    _PASS;
}
