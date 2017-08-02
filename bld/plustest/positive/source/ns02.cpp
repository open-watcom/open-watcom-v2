#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace {
    int x;
    int z = 3;
    int w = 4;
    int foo( int x, int y ) {
	return( w + x + y );
    }
};
int y = x++;

int main() {
    if( x != 1 ) fail(__LINE__);
    if( y != 0 ) fail(__LINE__);
    if( z != 3 ) fail(__LINE__);
    if( foo( 1, 2 ) != 7 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
