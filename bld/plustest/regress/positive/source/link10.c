#include "fail.h"

int got_here;

extern "C" {
    struct b {
	virtual int foo( int, int (*)( int ) ) = 0;
    };
}
struct a : b {
    virtual int foo( int, int (*)( int ) );
};
int a::foo( int x, int (*f)( int ) )
{
    ++got_here;
    return f(x);
}

int inc( int x )
{
    return x + 1;
}

extern "C" int dec( int x )
{
    return x - 1;
}

int main() {
    b *p = new a;
    if( p->foo( 0, inc ) != 1 ) fail(__LINE__);
    if( p->foo( 1, dec ) != 0 ) fail(__LINE__);
    if( got_here != 2 ) fail(__LINE__);
    delete p;
    _PASS;
}
