#include "fail.h"

int count;

struct CD {
    unsigned signature;
    CD(unsigned);
    ~CD();
};

CD::CD( unsigned line ) : signature(line)
{
    ++count;
}

CD::~CD()
{
    --count;
    if( count < 0 ) fail(__LINE__);
}

int foo( int x, int y, int z )
{
    return
	x ? (
	    CD(__LINE__),
	    y ? (
		CD(__LINE__),
		z ? (
		    CD(__LINE__),
		    7
		) : (
		    CD(__LINE__),
		    6
		)
	    ) : (
		CD(__LINE__),
		z ? (
		    CD(__LINE__),
		    5
		) : (
		    CD(__LINE__),
		    4
		)
	    )
	) : (
	    CD(__LINE__),
	    y ? (
		CD(__LINE__),
		z ? (
		    CD(__LINE__),
		    3
		) : (
		    CD(__LINE__),
		    2
		)
	    ) : (
		CD(__LINE__),
		z ? (
		    CD(__LINE__),
		    1
		) : (
		    CD(__LINE__),
		    0
		)
	    )
	);
}

void bar()
{
    #define paste(x,y)	x##y
    #define xpaste(x,y)	paste(x,y)
    #define __U	xpaste(__x,__LINE__)
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
    static CD __U(__LINE__);
}

int main()
{
    if( foo(0,0,0) != 0 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(0,0,1) != 1 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(0,1,0) != 2 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(0,1,1) != 3 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(1,0,0) != 4 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(1,0,1) != 5 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(1,1,0) != 6 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    if( foo(1,1,1) != 7 ) fail(__LINE__);
    if( count != 0 ) fail(__LINE__);
    count = 0;
    bar();
    if( count != 67 ) fail(__LINE__);
    _PASS;
}

struct __ {
    ~__() {
	if( count != 0 ) fail(__LINE__);
    }
};

__ ___;
