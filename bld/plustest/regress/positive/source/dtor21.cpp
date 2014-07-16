#include "fail.h"

int ctors;

struct CD {
    CD(int sig) : sig(sig) {
	++ctors;
    }
    ~CD() {
	--ctors;
	if( ctors < 0 ) fail(__LINE__);
    }
    int sig;
};

int check;

void use( int sig )
{
    if( sig != check ) fail(__LINE__);
    ++check;
}

void foo( int b )
{
    check = 1;
    CD const & v1 = CD(1);
    use( v1.sig );
    if( b ) {
        CD const & v2 = CD(2);
	use( v2.sig );
	if( b > 3 ) {
	    CD const & v3 = CD(3);
	    use( v3.sig );
	}
    }
}

int main()
{
    foo( 0 );
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    foo( 1 );
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    foo( 2 );
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    foo( 3 );
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    foo( 4 );
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    _PASS;
}
