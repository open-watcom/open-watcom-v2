#include "fail.h"

#ifdef __WATCOM_NAMESPACE__

namespace M {
    namespace N {
	int foo( int );
	int bar( int );
	int sam( int x ) {
	    return x + 's';
	}
	extern int x;
	extern int y;
	int z = 3;
    };
    int N::bar( int x ) {
	return x + 'b';
    }
    int N::x = 1;
};

int M::N::foo( int x ) {
    return x + 'f';
}
int M::N::y = 2;
#endif

int y = 'y';
int foo( int x ) {
    return x;
}

int main() {
    if( foo( 1 ) != 1 ) fail(__LINE__);
#ifdef __WATCOM_NAMESPACE__
    if( M::N::foo( 1 ) != 'g' ) fail(__LINE__);
    if( M::N::bar( 1 ) != 'c' ) fail(__LINE__);
    if( M::N::sam( 1 ) != 't' ) fail(__LINE__);
    if( M::N::x != 1 ) fail(__LINE__);
    if( M::N::y != 2 ) fail(__LINE__);
    if( M::N::z != 3 ) fail(__LINE__);
#endif
    if( y != 'y' ) fail(__LINE__);
    _PASS;
}
