#include "fail.h"

extern int _Optlink foo( int p1, int p2, int p3, int p4, int p5 )
{
    return p1 + p2 + p3 + p4 + p5;
}

#define xbit( i, b )	((i>>b)&1)

int main() {
    int s = 0;
    for( int i = 0; i < 10; ++i ) {
	s += foo( xbit(i,0), xbit(i,1), xbit(i,2), xbit(i,3), xbit(i,4) );
    }
    if( s != 15 ) fail(__LINE__);
    _PASS;
}
