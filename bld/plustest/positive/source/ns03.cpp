#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace {
    int foo( int x ) {
	return x + 1;
    }
}
namespace {
    char foo( char x ) {
	return x;
    }
}
int main() {
    if( foo( 0 ) != 1 ) fail(__LINE__);
    if( foo( '0' ) != '0' ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
