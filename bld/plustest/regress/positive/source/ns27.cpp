#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace a {
    int foo( int );
    char foo( char x ) {
	return x + 1;
    }
    double foo( double );
    int bar( int x ) {
	return -x;
    }
};
namespace b {
    using a::foo;
    using a::bar;
    int dummy;
    template <char (*f)( char ), int (*b)(int)>
	struct S {
	    int foo( char c ) {
		return b(f(c));
	    }
	    ~S() { ++dummy; }
	};
    template <char f( char ), int b(int)>
	struct R {
	    int foo( char c ) {
		return b(f(c));
	    }
	    ~R() { ++dummy; }
	};

    S<foo,bar> x;
    R<foo,bar> y;
};
using b::x;
using b::y;

int main() {
    if( x.foo( 'a' ) != -('a'+1) ) _fail;
    if( y.foo( 'b' ) != -('b'+1) ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
