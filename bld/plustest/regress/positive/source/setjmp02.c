#include "fail.h"
#if defined(__AXP__)
#include <setjmpex.h>
#else
#include <setjmp.h>
#endif

struct CD {
    static int count;
    int sig;
    CD() : sig(++count) {
    }
    CD(CD const &) : sig(++count) {
    }
    ~CD() {
	if( count != sig ) fail(__LINE__);
	--count;
	if( count < 0 ) fail(__LINE__);
    }
};
int CD::count;

jmp_buf info;


void bar( int x )
{
    if( x ) {
	longjmp( info, x );
    }
}

void force_xs() {
    throw 1;
}

void foo( int nz ) {
    CD x,y,z;
    bar(0);
    CD w,q;
    bar(nz);
}

int main() {
    {
	CD x;
	CD y;
	CD z;
	{
	    CD x,y,z;
	}
	if( ! x.sig ) {
	    fail(__LINE__);
	} else {
	    CD x,y,z;
#if defined(__AXP__)
	    int s = _setjmpex( info );
#else
	    int s = setjmp( info );
#endif
	    if( s == 0 ) {
		CD x,y,z;
		foo( __LINE__ );
	    } else {
		if( s != (__LINE__-2) ) fail(__LINE__);
	    }
	}
    }
    if( CD::count != 0 ) fail(__LINE__);
    _PASS;
}
