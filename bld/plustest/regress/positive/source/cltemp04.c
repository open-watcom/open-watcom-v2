#include "fail.h"

#if __WATCOM_REVISION >= 8
int count;

template <class T>
    struct S : _CD {
	S<T>( int x ) : x(x) {
	    ++count;
	}
	~S<T>() {
	    --count;
	}
	int x;
    };

S<int> x('x');
S<char> y('y');

int main() {
    if( x.x != 'x' ) fail(__LINE__);
    if( y.x != 'y' ) fail(__LINE__);
    if( count != 2 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
